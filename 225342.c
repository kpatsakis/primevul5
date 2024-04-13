void CServer::InitInterfaces(CConfig *pConfig, IConsole *pConsole, IGameServer *pGameServer, IEngineMap *pMap, IStorage *pStorage)
{
	m_pConfig = pConfig;
	m_pConsole = pConsole;
	m_pGameServer = pGameServer;
	m_pMap = pMap;
	m_pStorage = pStorage;
}