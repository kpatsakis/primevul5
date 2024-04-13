void CServer::SendMap(int ClientID)
{
	CMsgPacker Msg(NETMSG_MAP_CHANGE, true);
	Msg.AddString(GetMapName(), 0);
	Msg.AddInt(m_CurrentMapCrc);
	Msg.AddInt(m_CurrentMapSize);
	Msg.AddInt(m_MapChunksPerRequest);
	Msg.AddInt(MAP_CHUNK_SIZE);
	Msg.AddRaw(&m_CurrentMapSha256, sizeof(m_CurrentMapSha256));
	SendMsg(&Msg, MSGFLAG_VITAL|MSGFLAG_FLUSH, ClientID);
}