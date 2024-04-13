void CServer::InitRegister(CNetServer *pNetServer, IEngineMasterServer *pMasterServer, CConfig *pConfig, IConsole *pConsole)
{
	m_Register.Init(pNetServer, pMasterServer, pConfig, pConsole);
}