void CServer::SetClientName(int ClientID, const char *pName)
{
	if(ClientID < 0 || ClientID >= MAX_CLIENTS || m_aClients[ClientID].m_State < CClient::STATE_READY || !pName)
		return;

	const char *pDefaultName = "(1)";
	pName = str_utf8_skip_whitespaces(pName);
	str_copy(m_aClients[ClientID].m_aName, *pName ? pName : pDefaultName, MAX_NAME_LENGTH);
}