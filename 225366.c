void CServer::PumpNetwork()
{
	CNetChunk Packet;
	TOKEN ResponseToken;

	m_NetServer.Update();

	// process packets
	while(m_NetServer.Recv(&Packet, &ResponseToken))
	{
		if(Packet.m_Flags&NETSENDFLAG_CONNLESS)
		{
			if(m_Register.RegisterProcessPacket(&Packet, ResponseToken))
				continue;
			if(Packet.m_DataSize >= int(sizeof(SERVERBROWSE_GETINFO)) &&
				mem_comp(Packet.m_pData, SERVERBROWSE_GETINFO, sizeof(SERVERBROWSE_GETINFO)) == 0)
			{
				CUnpacker Unpacker;
				Unpacker.Reset((unsigned char*)Packet.m_pData+sizeof(SERVERBROWSE_GETINFO), Packet.m_DataSize-sizeof(SERVERBROWSE_GETINFO));
				int SrvBrwsToken = Unpacker.GetInt();
				if(Unpacker.Error())
					continue;

				CPacker Packer;
				CNetChunk Response;

				GenerateServerInfo(&Packer, SrvBrwsToken);

				Response.m_ClientID = -1;
				Response.m_Address = Packet.m_Address;
				Response.m_Flags = NETSENDFLAG_CONNLESS;
				Response.m_pData = Packer.Data();
				Response.m_DataSize = Packer.Size();
				m_NetServer.Send(&Response, ResponseToken);
			}
		}
		else
			ProcessClientPacket(&Packet);
	}

	m_ServerBan.Update();
	m_Econ.Update();
}