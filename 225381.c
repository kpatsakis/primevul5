bool CServer::IsAuthed(int ClientID) const
{
	return m_aClients[ClientID].m_Authed;
}