bool CServer::IsBanned(int ClientID)
{
	return m_ServerBan.IsBanned(m_NetServer.ClientAddr(ClientID), 0, 0, 0);
}