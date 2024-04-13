	bool NetworkPage(CWebSock& WebSock, CTemplate& Tmpl, CUser* pUser, CIRCNetwork* pNetwork = NULL) {
		CSmartPtr<CWebSession> spSession = WebSock.GetSession();
		Tmpl.SetFile("add_edit_network.tmpl");

		if (!WebSock.GetParam("submitted").ToUInt()) {
			Tmpl["Username"] = pUser->GetUserName();

			set<CModInfo> ssNetworkMods;
			CZNC::Get().GetModules().GetAvailableMods(ssNetworkMods, CModInfo::NetworkModule);
			for (set<CModInfo>::iterator it = ssNetworkMods.begin(); it != ssNetworkMods.end(); ++it) {
				const CModInfo& Info = *it;
				CTemplate& l = Tmpl.AddRow("ModuleLoop");

				l["Name"] = Info.GetName();
				l["Description"] = Info.GetDescription();
				l["Wiki"] = Info.GetWikiPage();
				l["HasArgs"] = CString(Info.GetHasArgs());
				l["ArgsHelpText"] = Info.GetArgsHelpText();

				if (pNetwork) {
					CModule *pModule = pNetwork->GetModules().FindModule(Info.GetName());
					if (pModule) {
						l["Checked"] = "true";
						l["Args"] = pModule->GetArgs();
					}
				}

				if (!spSession->IsAdmin() && pUser->DenyLoadMod()) {
					l["Disabled"] = "true";
				}
			}

			// To change BindHosts be admin or don't have DenySetBindHost
			if (spSession->IsAdmin() || !spSession->GetUser()->DenySetBindHost()) {
				Tmpl["BindHostEdit"] = "true";
				const VCString& vsBindHosts = CZNC::Get().GetBindHosts();
				if (vsBindHosts.empty()) {
					if (pNetwork) {
						Tmpl["BindHost"] = pNetwork->GetBindHost();
					}
				} else {
					bool bFoundBindHost = false;
					for (unsigned int b = 0; b < vsBindHosts.size(); b++) {
						const CString& sBindHost = vsBindHosts[b];
						CTemplate& l = Tmpl.AddRow("BindHostLoop");

						l["BindHost"] = sBindHost;

						if (pNetwork && pNetwork->GetBindHost() == sBindHost) {
							l["Checked"] = "true";
							bFoundBindHost = true;
						}
					}

					// If our current bindhost is not in the global list...
					if (pNetwork && !bFoundBindHost && !pNetwork->GetBindHost().empty()) {
						CTemplate& l = Tmpl.AddRow("BindHostLoop");

						l["BindHost"] = pNetwork->GetBindHost();
						l["Checked"] = "true";
					}
				}
			}

			if (pNetwork) {
				Tmpl["Action"] = "editnetwork";
				Tmpl["Edit"] = "true";
				Tmpl["Title"] = "Edit Network" + CString(" [" + pNetwork->GetName() + "]") + " of User [" + pUser->GetUserName() + "]";
				Tmpl["Name"] = pNetwork->GetName();

				Tmpl["Nick"] = pNetwork->GetNick();
				Tmpl["AltNick"] = pNetwork->GetAltNick();
				Tmpl["Ident"] = pNetwork->GetIdent();
				Tmpl["RealName"] = pNetwork->GetRealName();

				Tmpl["FloodProtection"] = CString(CIRCSock::IsFloodProtected(pNetwork->GetFloodRate()));
				Tmpl["FloodRate"] = CString(pNetwork->GetFloodRate());
				Tmpl["FloodBurst"] = CString(pNetwork->GetFloodBurst());

				Tmpl["IRCConnectEnabled"] = CString(pNetwork->GetIRCConnectEnabled());

				const vector<CServer*>& vServers = pNetwork->GetServers();
				for (unsigned int a = 0; a < vServers.size(); a++) {
					CTemplate& l = Tmpl.AddRow("ServerLoop");
					l["Server"] = vServers[a]->GetString();
				}

				const vector<CChan*>& Channels = pNetwork->GetChans();
				for (unsigned int c = 0; c < Channels.size(); c++) {
					CChan* pChan = Channels[c];
					CTemplate& l = Tmpl.AddRow("ChannelLoop");

					l["Network"] = pNetwork->GetName();
					l["Username"] = pUser->GetUserName();
					l["Name"] = pChan->GetName();
					l["Perms"] = pChan->GetPermStr();
					l["CurModes"] = pChan->GetModeString();
					l["DefModes"] = pChan->GetDefaultModes();
					l["BufferCount"] = CString(pChan->GetBufferCount());
					l["Options"] = pChan->GetOptions();

					if (pChan->InConfig()) {
						l["InConfig"] = "true";
					}
				}
			} else {
				if (!spSession->IsAdmin() && !pUser->HasSpaceForNewNetwork()) {
					WebSock.PrintErrorPage("Network number limit reached. Ask an admin to increase the limit for you, or delete few old ones from Your Settings");
					return true;
				}

				Tmpl["Action"] = "addnetwork";
				Tmpl["Title"] = "Add Network for User [" + pUser->GetUserName() + "]";
				Tmpl["IRCConnectEnabled"] = "true";
				Tmpl["FloodProtection"] = "true";
				Tmpl["FloodRate"] = "1.0";
				Tmpl["FloodBurst"] = "4";
			}

			FOR_EACH_MODULE(i, make_pair(pUser, pNetwork)) {
				CTemplate& mod = Tmpl.AddRow("EmbeddedModuleLoop");
				mod.insert(Tmpl.begin(), Tmpl.end());
				mod["WebadminAction"] = "display";
				if ((*i)->OnEmbeddedWebRequest(WebSock, "webadmin/network", mod)) {
					mod["Embed"] = WebSock.FindTmpl(*i, "WebadminNetwork.tmpl");
					mod["ModName"] = (*i)->GetModName();
				}
			}

			return true;
		}

		CString sName = WebSock.GetParam("network").Trim_n();
		if (sName.empty()) {
			WebSock.PrintErrorPage("Network name is a required argument");
			return true;
		}

		if (!pNetwork) {
			if (!spSession->IsAdmin() && !pUser->HasSpaceForNewNetwork()) {
				WebSock.PrintErrorPage("Network number limit reached. Ask an admin to increase the limit for you, or delete few old ones from Your Settings");
				return true;
			}
			if (!CIRCNetwork::IsValidNetwork(sName)) {
				WebSock.PrintErrorPage("Network name should be alphanumeric");
				return true;
			}
			pNetwork = pUser->AddNetwork(sName);
			if (!pNetwork) {
				WebSock.PrintErrorPage("Network [" + sName.Token(0) + "] already exists");
				return true;
			}
		}

		CString sArg;

		pNetwork->SetNick(WebSock.GetParam("nick"));
		pNetwork->SetAltNick(WebSock.GetParam("altnick"));
		pNetwork->SetIdent(WebSock.GetParam("ident"));
		pNetwork->SetRealName(WebSock.GetParam("realname"));

		pNetwork->SetIRCConnectEnabled(WebSock.GetParam("doconnect").ToBool());

		sArg = WebSock.GetParam("bindhost");
		// To change BindHosts be admin or don't have DenySetBindHost
		if (spSession->IsAdmin() || !spSession->GetUser()->DenySetBindHost()) {
			CString sHost = WebSock.GetParam("bindhost");
			const VCString& vsHosts = CZNC::Get().GetBindHosts();
			if (!spSession->IsAdmin() && !vsHosts.empty()) {
				VCString::const_iterator it;
				bool bFound = false;

				for (it = vsHosts.begin(); it != vsHosts.end(); ++it) {
					if (sHost.Equals(*it)) {
						bFound = true;
						break;
					}
				}

				if (!bFound) {
					sHost = pNetwork->GetBindHost();
				}
			}
			pNetwork->SetBindHost(sHost);
		}

		if (WebSock.GetParam("floodprotection").ToBool()) {
			pNetwork->SetFloodRate(WebSock.GetParam("floodrate").ToDouble());
			pNetwork->SetFloodBurst(WebSock.GetParam("floodburst").ToUShort());
		} else {
			pNetwork->SetFloodRate(-1);
		}

		VCString vsArgs;

		pNetwork->DelServers();
		WebSock.GetRawParam("servers").Split("\n", vsArgs);
		for (unsigned int a = 0; a < vsArgs.size(); a++) {
			pNetwork->AddServer(vsArgs[a].Trim_n());
		}

		WebSock.GetParamValues("channel", vsArgs);
		for (unsigned int a = 0; a < vsArgs.size(); a++) {
			const CString& sChan = vsArgs[a];
			CChan *pChan = pNetwork->FindChan(sChan.TrimRight_n("\r"));
			if (pChan) {
				pChan->SetInConfig(WebSock.GetParam("save_" + sChan).ToBool());
			}
		}

		set<CString> ssArgs;
		WebSock.GetParamValues("loadmod", ssArgs);
		if (spSession->IsAdmin() || !pUser->DenyLoadMod()) {
			for (set<CString>::iterator it = ssArgs.begin(); it != ssArgs.end(); ++it) {
				CString sModRet;
				CString sModName = (*it).TrimRight_n("\r");
				CString sModLoadError;

				if (!sModName.empty()) {
					CString sArgs = WebSock.GetParam("modargs_" + sModName);

					CModule *pMod = pNetwork->GetModules().FindModule(sModName);

					if (!pMod) {
						if (!pNetwork->GetModules().LoadModule(sModName, sArgs, CModInfo::NetworkModule, pUser, pNetwork, sModRet)) {
							sModLoadError = "Unable to load module [" + sModName + "] [" + sModRet + "]";
						}
					} else if (pMod->GetArgs() != sArgs) {
						if (!pNetwork->GetModules().ReloadModule(sModName, sArgs, pUser, pNetwork, sModRet)) {
							sModLoadError = "Unable to reload module [" + sModName + "] [" + sModRet + "]";
						}
					}

					if (!sModLoadError.empty()) {
						DEBUG(sModLoadError);
						WebSock.GetSession()->AddError(sModLoadError);
					}
				}
			}
		}

		const CModules& vCurMods = pNetwork->GetModules();
		set<CString> ssUnloadMods;

		for (unsigned int a = 0; a < vCurMods.size(); a++) {
			CModule* pCurMod = vCurMods[a];

			if (ssArgs.find(pCurMod->GetModName()) == ssArgs.end() && pCurMod->GetModName() != GetModName()) {
				ssUnloadMods.insert(pCurMod->GetModName());
			}
		}

		for (set<CString>::iterator it2 = ssUnloadMods.begin(); it2 != ssUnloadMods.end(); ++it2) {
			pNetwork->GetModules().UnloadModule(*it2);
		}

		CTemplate TmplMod;
		TmplMod["Username"] = pUser->GetUserName();
		TmplMod["Name"] = pNetwork->GetName();
		TmplMod["WebadminAction"] = "change";
		FOR_EACH_MODULE(it, make_pair(pUser, pNetwork)) {
			(*it)->OnEmbeddedWebRequest(WebSock, "webadmin/network", TmplMod);
		}

		if (!CZNC::Get().WriteConfig()) {
			WebSock.PrintErrorPage("Network added/modified, but config was not written");
			return true;
		}

		WebSock.Redirect("edituser?user=" + pUser->GetUserName().Escape_n(CString::EURL));
		return true;
	}