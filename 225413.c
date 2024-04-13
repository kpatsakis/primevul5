void CServer::SendServerInfo(int ClientID)
{
	CMsgPacker Msg(NETMSG_SERVERINFO, true);
	GenerateServerInfo(&Msg, -1);
	if(ClientID == -1)
	{
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(m_aClients[i].m_State != CClient::STATE_EMPTY)
				SendMsg(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH, i);
		}
	}
	else if(ClientID >= 0 && ClientID < MAX_CLIENTS && m_aClients[ClientID].m_State != CClient::STATE_EMPTY)
		SendMsg(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH, ClientID);
}