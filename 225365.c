void CServer::UpdateClientMapListEntries()
{
	for(int ClientID = Tick() % MAX_RCONCMD_RATIO; ClientID < MAX_CLIENTS; ClientID += MAX_RCONCMD_RATIO)
	{
		if(m_aClients[ClientID].m_State != CClient::STATE_EMPTY && m_aClients[ClientID].m_Authed)
		{
			for(int i = 0; i < MAX_MAPLISTENTRY_SEND && m_aClients[ClientID].m_pMapListEntryToSend; ++i)
			{
				SendMapListEntryAdd(m_aClients[ClientID].m_pMapListEntryToSend, ClientID);
				m_aClients[ClientID].m_pMapListEntryToSend = m_aClients[ClientID].m_pMapListEntryToSend->m_pNext;
			}
		}
	}
}