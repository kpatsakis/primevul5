	CUser* GetNewUser(CWebSock& WebSock, CUser* pUser) {
		CSmartPtr<CWebSession> spSession = WebSock.GetSession();
		CString sUsername = WebSock.GetParam("newuser");

		if (sUsername.empty()) {
			sUsername = WebSock.GetParam("user");
		}

		if (sUsername.empty()) {
			WebSock.PrintErrorPage("Invalid Submission [Username is required]");
			return NULL;
		}

		if (pUser) {
			/* If we are editing a user we must not change the user name */
			sUsername = pUser->GetUserName();
		}

		CString sArg = WebSock.GetParam("password");

		if (sArg != WebSock.GetParam("password2")) {
			WebSock.PrintErrorPage("Invalid Submission [Passwords do not match]");
			return NULL;
		}

		CUser* pNewUser = new CUser(sUsername);

		if (!sArg.empty()) {
			CString sSalt = CUtils::GetSalt();
			CString sHash = CUser::SaltedHash(sArg, sSalt);
			pNewUser->SetPass(sHash, CUser::HASH_DEFAULT, sSalt);
		}

		VCString vsArgs;
		unsigned int a = 0;

		WebSock.GetRawParam("allowedips").Split("\n", vsArgs);
		if (vsArgs.size()) {
			for (a = 0; a < vsArgs.size(); a++) {
				pNewUser->AddAllowedHost(vsArgs[a].Trim_n());
			}
		} else {
			pNewUser->AddAllowedHost("*");
		}

		WebSock.GetRawParam("ctcpreplies").Split("\n", vsArgs);
		for (a = 0; a < vsArgs.size(); a++) {
			CString sReply = vsArgs[a].TrimRight_n("\r");
			pNewUser->AddCTCPReply(sReply.Token(0).Trim_n(), sReply.Token(1, true).Trim_n());
		}

		sArg = WebSock.GetParam("nick"); if (!sArg.empty()) { pNewUser->SetNick(sArg); }
		sArg = WebSock.GetParam("altnick"); if (!sArg.empty()) { pNewUser->SetAltNick(sArg); }
		sArg = WebSock.GetParam("statusprefix"); if (!sArg.empty()) { pNewUser->SetStatusPrefix(sArg); }
		sArg = WebSock.GetParam("ident"); if (!sArg.empty()) { pNewUser->SetIdent(sArg); }
		sArg = WebSock.GetParam("skin"); if (!sArg.empty()) { pNewUser->SetSkinName(sArg); }
		sArg = WebSock.GetParam("realname"); if (!sArg.empty()) { pNewUser->SetRealName(sArg); }
		sArg = WebSock.GetParam("quitmsg"); if (!sArg.empty()) { pNewUser->SetQuitMsg(sArg); }
		sArg = WebSock.GetParam("chanmodes"); if (!sArg.empty()) { pNewUser->SetDefaultChanModes(sArg); }
		sArg = WebSock.GetParam("timestampformat"); if (!sArg.empty()) { pNewUser->SetTimestampFormat(sArg); }

		sArg = WebSock.GetParam("bindhost");
		// To change BindHosts be admin or don't have DenySetBindHost
		if (spSession->IsAdmin() || !spSession->GetUser()->DenySetBindHost()) {
			CString sArg2 = WebSock.GetParam("dccbindhost");
			if (!sArg.empty()) {
				pNewUser->SetBindHost(sArg);
			}
			if (!sArg2.empty()) {
				pNewUser->SetDCCBindHost(sArg2);
			}

			const VCString& vsHosts = CZNC::Get().GetBindHosts();
			if (!spSession->IsAdmin() && !vsHosts.empty()) {
				VCString::const_iterator it;
				bool bFound = false;
				bool bFoundDCC = false;

				for (it = vsHosts.begin(); it != vsHosts.end(); ++it) {
					if (sArg.Equals(*it)) {
						bFound = true;
					}
					if (sArg2.Equals(*it)) {
						bFoundDCC = true;
					}
				}

				if (!bFound) {
					pNewUser->SetBindHost(pUser ? pUser->GetBindHost() : "");
				}
				if (!bFoundDCC) {
					pNewUser->SetDCCBindHost(pUser ? pUser->GetDCCBindHost() : "");
				}
			}
		} else if (pUser){
			pNewUser->SetBindHost(pUser->GetBindHost());
			pNewUser->SetDCCBindHost(pUser->GetDCCBindHost());
		}

		sArg = WebSock.GetParam("bufsize"); if (!sArg.empty()) pNewUser->SetBufferCount(sArg.ToUInt(), spSession->IsAdmin());
		if (!sArg.empty()) {
			// First apply the old limit in case the new one is too high
			if (pUser)
				pNewUser->SetBufferCount(pUser->GetBufferCount(), true);
			pNewUser->SetBufferCount(sArg.ToUInt(), spSession->IsAdmin());
		}

		pNewUser->SetSkinName(WebSock.GetParam("skin"));
		pNewUser->SetAutoClearChanBuffer(WebSock.GetParam("autoclearchanbuffer").ToBool());
		pNewUser->SetMultiClients(WebSock.GetParam("multiclients").ToBool());
		pNewUser->SetTimestampAppend(WebSock.GetParam("appendtimestamp").ToBool());
		pNewUser->SetTimestampPrepend(WebSock.GetParam("prependtimestamp").ToBool());
		pNewUser->SetTimezone(WebSock.GetParam("timezone"));
		pNewUser->SetJoinTries(WebSock.GetParam("jointries").ToUInt());

		if (spSession->IsAdmin()) {
			pNewUser->SetDenyLoadMod(WebSock.GetParam("denyloadmod").ToBool());
			pNewUser->SetDenySetBindHost(WebSock.GetParam("denysetbindhost").ToBool());
			sArg = WebSock.GetParam("maxnetworks"); if (!sArg.empty()) pNewUser->SetMaxNetworks(sArg.ToUInt());
		} else if (pUser) {
			pNewUser->SetDenyLoadMod(pUser->DenyLoadMod());
			pNewUser->SetDenySetBindHost(pUser->DenySetBindHost());
			pNewUser->SetMaxNetworks(pUser->MaxNetworks());
		}

		// If pUser is not NULL, we are editing an existing user.
		// Users must not be able to change their own admin flag.
		if (pUser != CZNC::Get().FindUser(WebSock.GetUser())) {
			pNewUser->SetAdmin(WebSock.GetParam("isadmin").ToBool());
		} else if (pUser) {
			pNewUser->SetAdmin(pUser->IsAdmin());
		}

		if (spSession->IsAdmin() || (pUser && !pUser->DenyLoadMod())) {
			WebSock.GetParamValues("loadmod", vsArgs);

			// disallow unload webadmin from itself
			if (CModInfo::UserModule == GetType() && pUser == CZNC::Get().FindUser(WebSock.GetUser())) {
				bool bLoadedWebadmin = false;
				for (a = 0; a < vsArgs.size(); ++a) {
					CString sModName = vsArgs[a].TrimRight_n("\r");
					if (sModName == GetModName()) {
						bLoadedWebadmin = true;
						break;
					}
				}
				if (!bLoadedWebadmin) {
					vsArgs.push_back(GetModName());
				}
			}

			for (a = 0; a < vsArgs.size(); a++) {
				CString sModRet;
				CString sModName = vsArgs[a].TrimRight_n("\r");
				CString sModLoadError;

				if (!sModName.empty()) {
					CString sArgs = WebSock.GetParam("modargs_" + sModName);

					try {
						if (!pNewUser->GetModules().LoadModule(sModName, sArgs, CModInfo::UserModule, pNewUser, NULL, sModRet)) {
							sModLoadError = "Unable to load module [" + sModName + "] [" + sModRet + "]";
						}
					} catch (...) {
						sModLoadError = "Unable to load module [" + sModName + "] [" + sArgs + "]";
					}

					if (!sModLoadError.empty()) {
						DEBUG(sModLoadError);
						spSession->AddError(sModLoadError);
					}
				}
			}
		} else if (pUser) {
			CModules& Modules = pUser->GetModules();

			for (a = 0; a < Modules.size(); a++) {
				CString sModName = Modules[a]->GetModName();
				CString sArgs = Modules[a]->GetArgs();
				CString sModRet;
				CString sModLoadError;

				try {
					if (!pNewUser->GetModules().LoadModule(sModName, sArgs, CModInfo::UserModule, pNewUser, NULL, sModRet)) {
						sModLoadError = "Unable to load module [" + sModName + "] [" + sModRet + "]";
					}
				} catch (...) {
					sModLoadError = "Unable to load module [" + sModName + "]";
				}

				if (!sModLoadError.empty()) {
					DEBUG(sModLoadError);
					spSession->AddError(sModLoadError);
				}
			}
		}

		return pNewUser;
	}