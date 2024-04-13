int CServer::Run()
{
	//
	m_PrintCBIndex = Console()->RegisterPrintCallback(Config()->m_ConsoleOutputLevel, SendRconLineAuthed, this);

	// list maps
	m_pMapListHeap = new CHeap();
	CSubdirCallbackUserdata Userdata;
	Userdata.m_pServer = this;
	str_copy(Userdata.m_aName, "", sizeof(Userdata.m_aName));
	m_pStorage->ListDirectory(IStorage::TYPE_ALL, "maps/", MapListEntryCallback, &Userdata);

	// load map
	if(!LoadMap(Config()->m_SvMap))
	{
		dbg_msg("server", "failed to load map. mapname='%s'", Config()->m_SvMap);
		return -1;
	}
	m_MapChunksPerRequest = Config()->m_SvMapDownloadSpeed;

	// start server
	NETADDR BindAddr;
	if(Config()->m_Bindaddr[0] && net_host_lookup(Config()->m_Bindaddr, &BindAddr, NETTYPE_ALL) == 0)
	{
		// sweet!
		BindAddr.type = NETTYPE_ALL;
		BindAddr.port = Config()->m_SvPort;
	}
	else
	{
		mem_zero(&BindAddr, sizeof(BindAddr));
		BindAddr.type = NETTYPE_ALL;
		BindAddr.port = Config()->m_SvPort;
	}

	if(!m_NetServer.Open(BindAddr, Config(), Console(), Kernel()->RequestInterface<IEngine>(), &m_ServerBan,
		Config()->m_SvMaxClients, Config()->m_SvMaxClientsPerIP, NewClientCallback, DelClientCallback, this))
	{
		dbg_msg("server", "couldn't open socket. port %d might already be in use", Config()->m_SvPort);
		return -1;
	}

	m_Econ.Init(Config(), Console(), &m_ServerBan);

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "server name is '%s'", Config()->m_SvName);
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);

	GameServer()->OnInit();
	str_format(aBuf, sizeof(aBuf), "version %s", GameServer()->NetVersion());
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
	if(str_comp(GameServer()->NetVersionHashUsed(), GameServer()->NetVersionHashReal()))
	{
		m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", "WARNING: netversion hash differs");
	}

	// process pending commands
	m_pConsole->StoreCommands(false);

	if(m_GeneratedRconPassword)
	{
		dbg_msg("server", "+-------------------------+");
		dbg_msg("server", "| rcon password: '%s' |", Config()->m_SvRconPassword);
		dbg_msg("server", "+-------------------------+");
	}

	// start game
	{
		int64 ReportTime = time_get();
		int ReportInterval = 3;

		m_Lastheartbeat = 0;
		m_GameStartTime = time_get();

		while(m_RunServer)
		{
			int64 t = time_get();
			int NewTicks = 0;

			// load new map TODO: don't poll this
			if(str_comp(Config()->m_SvMap, m_aCurrentMap) != 0 || m_MapReload || m_CurrentGameTick >= 0x6FFFFFFF) //	force reload to make sure the ticks stay within a valid range
			{
				m_MapReload = 0;

				// load map
				if(LoadMap(Config()->m_SvMap))
				{
					// new map loaded
					bool aSpecs[MAX_CLIENTS];
					for(int c = 0; c < MAX_CLIENTS; c++)
						aSpecs[c] = GameServer()->IsClientSpectator(c);

					GameServer()->OnShutdown();

					for(int c = 0; c < MAX_CLIENTS; c++)
					{
						if(m_aClients[c].m_State <= CClient::STATE_AUTH)
							continue;

						SendMap(c);
						m_aClients[c].Reset();
						m_aClients[c].m_State = aSpecs[c] ? CClient::STATE_CONNECTING_AS_SPEC : CClient::STATE_CONNECTING;
					}

					m_GameStartTime = time_get();
					m_CurrentGameTick = 0;
					Kernel()->ReregisterInterface(GameServer());
					GameServer()->OnInit();
				}
				else
				{
					str_format(aBuf, sizeof(aBuf), "failed to load map. mapname='%s'", Config()->m_SvMap);
					Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
					str_copy(Config()->m_SvMap, m_aCurrentMap, sizeof(Config()->m_SvMap));
				}
			}

			while(t > TickStartTime(m_CurrentGameTick+1))
			{
				m_CurrentGameTick++;
				NewTicks++;

				// apply new input
				for(int c = 0; c < MAX_CLIENTS; c++)
				{
					if(m_aClients[c].m_State == CClient::STATE_EMPTY)
						continue;
					for(int i = 0; i < 200; i++)
					{
						if(m_aClients[c].m_aInputs[i].m_GameTick == Tick())
						{
							if(m_aClients[c].m_State == CClient::STATE_INGAME)
								GameServer()->OnClientPredictedInput(c, m_aClients[c].m_aInputs[i].m_aData);
							break;
						}
					}
				}

				GameServer()->OnTick();
			}

			// snap game
			if(NewTicks)
			{
				if(Config()->m_SvHighBandwidth || (m_CurrentGameTick%2) == 0)
					DoSnapshot();

				UpdateClientRconCommands();
				UpdateClientMapListEntries();
			}

			// master server stuff
			m_Register.RegisterUpdate(m_NetServer.NetType());

			PumpNetwork();

			if(ReportTime < time_get())
			{
				if(Config()->m_Debug)
				{
					/*
					static NETSTATS prev_stats;
					NETSTATS stats;
					netserver_stats(net, &stats);

					perf_next();

					if(config.dbg_pref)
						perf_dump(&rootscope);

					dbg_msg("server", "send=%8d recv=%8d",
						(stats.send_bytes - prev_stats.send_bytes)/reportinterval,
						(stats.recv_bytes - prev_stats.recv_bytes)/reportinterval);

					prev_stats = stats;
					*/
				}

				ReportTime += time_freq()*ReportInterval;
			}

			// wait for incomming data
			m_NetServer.Wait(5);
		}
	}
	// disconnect all clients on shutdown
	m_NetServer.Close();
	m_Econ.Shutdown();

	GameServer()->OnShutdown();
	m_pMap->Unload();

	if(m_pCurrentMapData)
	{
		mem_free(m_pCurrentMapData);
		m_pCurrentMapData = 0;
	}
	if(m_pMapListHeap)
	{
		delete m_pMapListHeap;
		m_pMapListHeap = 0;
	}
	return 0;
}