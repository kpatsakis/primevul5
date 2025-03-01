CServer::CServer() : m_DemoRecorder(&m_SnapshotDelta)
{
	m_TickSpeed = SERVER_TICK_SPEED;

	m_pGameServer = 0;

	m_CurrentGameTick = 0;
	m_RunServer = 1;

	m_pCurrentMapData = 0;
	m_CurrentMapSize = 0;

	m_NumMapEntries = 0;
	m_pFirstMapEntry = 0;
	m_pLastMapEntry = 0;
	m_pMapListHeap = 0;

	m_MapReload = 0;

	m_RconClientID = IServer::RCON_CID_SERV;
	m_RconAuthLevel = AUTHED_ADMIN;

	m_RconPasswordSet = 0;
	m_GeneratedRconPassword = 0;

	Init();
}