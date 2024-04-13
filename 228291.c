	bool UserPage(CWebSock& WebSock, CTemplate& Tmpl, CUser* pUser = NULL) {
		CSmartPtr<CWebSession> spSession = WebSock.GetSession();
		Tmpl.SetFile("add_edit_user.tmpl");

		if (!WebSock.GetParam("submitted").ToUInt()) {
			CString sAllowedHosts, sServers, sChans, sCTCPReplies;

			if (pUser) {
				Tmpl["Action"] = "edituser";
				Tmpl["Title"] = "Edit User [" + pUser->GetUserName() + "]";
				Tmpl["Edit"] = "true";
			} else {
				CString sUsername = WebSock.GetParam("clone", false);
				pUser = CZNC::Get().FindUser(sUsername);

				if (pUser) {
					Tmpl["Title"] = "Clone User [" + pUser->GetUserName() + "]";
					Tmpl["Clone"] = "true";
					Tmpl["CloneUsername"] = pUser->GetUserName();
				}
			}

			Tmpl["ImAdmin"] = CString(spSession->IsAdmin());

			if (pUser) {
				Tmpl["Username"] = pUser->GetUserName();
				Tmpl["Nick"] = pUser->GetNick();
				Tmpl["AltNick"] = pUser->GetAltNick();
				Tmpl["StatusPrefix"] = pUser->GetStatusPrefix();
				Tmpl["Ident"] = pUser->GetIdent();
				Tmpl["RealName"] = pUser->GetRealName();
				Tmpl["QuitMsg"] = pUser->GetQuitMsg();
				Tmpl["DefaultChanModes"] = pUser->GetDefaultChanModes();
				Tmpl["BufferCount"] = CString(pUser->GetBufferCount());
				Tmpl["TimestampFormat"] = pUser->GetTimestampFormat();
				Tmpl["Timezone"] = pUser->GetTimezone();
				Tmpl["JoinTries"] = CString(pUser->JoinTries());
				Tmpl["MaxNetworks"] = CString(pUser->MaxNetworks());

				const set<CString>& ssAllowedHosts = pUser->GetAllowedHosts();
				for (set<CString>::const_iterator it = ssAllowedHosts.begin(); it != ssAllowedHosts.end(); ++it) {
					CTemplate& l = Tmpl.AddRow("AllowedHostLoop");
					l["Host"] = *it;
				}

				const vector<CIRCNetwork*>& vNetworks = pUser->GetNetworks();
				for (unsigned int a = 0; a < vNetworks.size(); a++) {
					CTemplate& l = Tmpl.AddRow("NetworkLoop");
					l["Name"] = vNetworks[a]->GetName();
					l["Username"] = pUser->GetUserName();
					l["Clients"] = CString(vNetworks[a]->GetClients().size());
					l["IRCNick"] = vNetworks[a]->GetIRCNick().GetNick();
					CServer* pServer = vNetworks[a]->GetCurrentServer();
					if (pServer) {
						l["Server"] = pServer->GetName();
					}
				}

				const MCString& msCTCPReplies = pUser->GetCTCPReplies();
				for (MCString::const_iterator it2 = msCTCPReplies.begin(); it2 != msCTCPReplies.end(); ++it2) {
					CTemplate& l = Tmpl.AddRow("CTCPLoop");
					l["CTCP"] = it2->first + " " + it2->second;
				}
			} else {
				Tmpl["Action"] = "adduser";
				Tmpl["Title"] = "Add User";
				Tmpl["StatusPrefix"] = "*";
			}

			SCString ssTimezones = CUtils::GetTimezones();
			for (SCString::iterator i = ssTimezones.begin(); i != ssTimezones.end(); ++i) {
				CTemplate& l = Tmpl.AddRow("TZLoop");
				l["TZ"] = *i;
			}

			// To change BindHosts be admin or don't have DenySetBindHost
			if (spSession->IsAdmin() || !spSession->GetUser()->DenySetBindHost()) {
				Tmpl["BindHostEdit"] = "true";
				const VCString& vsBindHosts = CZNC::Get().GetBindHosts();
				if (vsBindHosts.empty()) {
					if (pUser) {
						Tmpl["BindHost"] = pUser->GetBindHost();
						Tmpl["DCCBindHost"] = pUser->GetDCCBindHost();
					}
				} else {
					bool bFoundBindHost = false;
					bool bFoundDCCBindHost = false;
					for (unsigned int b = 0; b < vsBindHosts.size(); b++) {
						const CString& sBindHost = vsBindHosts[b];
						CTemplate& l = Tmpl.AddRow("BindHostLoop");
						CTemplate& k = Tmpl.AddRow("DCCBindHostLoop");

						l["BindHost"] = sBindHost;
						k["BindHost"] = sBindHost;

						if (pUser && pUser->GetBindHost() == sBindHost) {
							l["Checked"] = "true";
							bFoundBindHost = true;
						}

						if (pUser && pUser->GetDCCBindHost() == sBindHost) {
							k["Checked"] = "true";
							bFoundDCCBindHost = true;
						}
					}

					// If our current bindhost is not in the global list...
					if (pUser && !bFoundBindHost && !pUser->GetBindHost().empty()) {
						CTemplate& l = Tmpl.AddRow("BindHostLoop");

						l["BindHost"] = pUser->GetBindHost();
						l["Checked"] = "true";
					}
					if (pUser && !bFoundDCCBindHost && !pUser->GetDCCBindHost().empty()) {
						CTemplate& l = Tmpl.AddRow("DCCBindHostLoop");

						l["BindHost"] = pUser->GetDCCBindHost();
						l["Checked"] = "true";
					}
				}
			}

			vector<CString> vDirs;
			WebSock.GetAvailSkins(vDirs);

			for (unsigned int d = 0; d < vDirs.size(); d++) {
				const CString& SubDir = vDirs[d];
				CTemplate& l = Tmpl.AddRow("SkinLoop");
				l["Name"] = SubDir;

				if (pUser && SubDir == pUser->GetSkinName()) {
					l["Checked"] = "true";
				}
			}

			set<CModInfo> ssUserMods;
			CZNC::Get().GetModules().GetAvailableMods(ssUserMods);

			for (set<CModInfo>::iterator it = ssUserMods.begin(); it != ssUserMods.end(); ++it) {
				const CModInfo& Info = *it;
				CTemplate& l = Tmpl.AddRow("ModuleLoop");

				l["Name"] = Info.GetName();
				l["Description"] = Info.GetDescription();
				l["Wiki"] = Info.GetWikiPage();
				l["HasArgs"] = CString(Info.GetHasArgs());
				l["ArgsHelpText"] = Info.GetArgsHelpText();

				CModule *pModule = NULL;
				if (pUser)
					pModule = pUser->GetModules().FindModule(Info.GetName());
				if (pModule) {
					l["Checked"] = "true";
					l["Args"] = pModule->GetArgs();
					if (CModInfo::UserModule == GetType() && Info.GetName() == GetModName()) {
						l["Disabled"] = "true";
					}
				}

				if (!spSession->IsAdmin() && pUser && pUser->DenyLoadMod()) {
					l["Disabled"] = "true";
				}
			}

			CTemplate& o1 = Tmpl.AddRow("OptionLoop");
			o1["Name"] = "autoclearchanbuffer";
			o1["DisplayName"] = "Auto Clear Chan Buffer";
			o1["Tooltip"] = "Automatically Clear Channel Buffer After Playback (the default value for new channels)";
			if (!pUser || pUser->AutoClearChanBuffer()) { o1["Checked"] = "true"; }

			/* o2 used to be auto cycle which was removed */

			CTemplate& o4 = Tmpl.AddRow("OptionLoop");
			o4["Name"] = "multiclients";
			o4["DisplayName"] = "Multi Clients";
			if (!pUser || pUser->MultiClients()) { o4["Checked"] = "true"; }

			CTemplate& o7 = Tmpl.AddRow("OptionLoop");
			o7["Name"] = "appendtimestamp";
			o7["DisplayName"] = "Append Timestamps";
			if (pUser && pUser->GetTimestampAppend()) { o7["Checked"] = "true"; }

			CTemplate& o8 = Tmpl.AddRow("OptionLoop");
			o8["Name"] = "prependtimestamp";
			o8["DisplayName"] = "Prepend Timestamps";
			if (pUser && pUser->GetTimestampPrepend()) { o8["Checked"] = "true"; }

			if (spSession->IsAdmin()) {
				CTemplate& o9 = Tmpl.AddRow("OptionLoop");
				o9["Name"] = "denyloadmod";
				o9["DisplayName"] = "Deny LoadMod";
				if (pUser && pUser->DenyLoadMod()) { o9["Checked"] = "true"; }

				CTemplate& o10 = Tmpl.AddRow("OptionLoop");
				o10["Name"] = "isadmin";
				o10["DisplayName"] = "Admin";
				if (pUser && pUser->IsAdmin()) { o10["Checked"] = "true"; }
				if (pUser && pUser == CZNC::Get().FindUser(WebSock.GetUser())) { o10["Disabled"] = "true"; }

				CTemplate& o11 = Tmpl.AddRow("OptionLoop");
				o11["Name"] = "denysetbindhost";
				o11["DisplayName"] = "Deny SetBindHost";
				if (pUser && pUser->DenySetBindHost()) { o11["Checked"] = "true"; }
			}

			FOR_EACH_MODULE(i, pUser) {
				CTemplate& mod = Tmpl.AddRow("EmbeddedModuleLoop");
				mod.insert(Tmpl.begin(), Tmpl.end());
				mod["WebadminAction"] = "display";
				if ((*i)->OnEmbeddedWebRequest(WebSock, "webadmin/user", mod)) {
					mod["Embed"] = WebSock.FindTmpl(*i, "WebadminUser.tmpl");
					mod["ModName"] = (*i)->GetModName();
				}
			}

			return true;
		}

		/* If pUser is NULL, we are adding a user, else we are editing this one */

		CString sUsername = WebSock.GetParam("user");
		if (!pUser && CZNC::Get().FindUser(sUsername)) {
			WebSock.PrintErrorPage("Invalid Submission [User " + sUsername + " already exists]");
			return true;
		}

		CUser* pNewUser = GetNewUser(WebSock, pUser);
		if (!pNewUser) {
			WebSock.PrintErrorPage("Invalid user settings");
			return true;
		}

		CString sErr;
		CString sAction;

		if (!pUser) {
			CString sClone = WebSock.GetParam("clone");
			if (CUser *pCloneUser = CZNC::Get().FindUser(sClone)) {
				pNewUser->CloneNetworks(*pCloneUser);
			}

			// Add User Submission
			if (!CZNC::Get().AddUser(pNewUser, sErr)) {
				delete pNewUser;
				WebSock.PrintErrorPage("Invalid submission [" + sErr + "]");
				return true;
			}

			pUser = pNewUser;
			sAction = "added";
		} else {
			// Edit User Submission
			if (!pUser->Clone(*pNewUser, sErr, false)) {
				delete pNewUser;
				WebSock.PrintErrorPage("Invalid Submission [" + sErr + "]");
				return true;
			}

			delete pNewUser;
			sAction = "edited";
		}

		CTemplate TmplMod;
		TmplMod["Username"] = sUsername;
		TmplMod["WebadminAction"] = "change";
		FOR_EACH_MODULE(it, pUser) {
			(*it)->OnEmbeddedWebRequest(WebSock, "webadmin/user", TmplMod);
		}

		if (!CZNC::Get().WriteConfig()) {
			WebSock.PrintErrorPage("User " + sAction + ", but config was not written");
			return true;
		}

		if (!spSession->IsAdmin()) {
			WebSock.Redirect("edituser");
		} else {
			WebSock.Redirect("listusers");
		}

		/* we don't want the template to be printed while we redirect */
		return false;
	}