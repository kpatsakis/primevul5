const char *CServer::GetMapName()
{
	// get the name of the map without his path
	char *pMapShortName = &Config()->m_SvMap[0];
	for(int i = 0; i < str_length(Config()->m_SvMap)-1; i++)
	{
		if(Config()->m_SvMap[i] == '/' || Config()->m_SvMap[i] == '\\')
			pMapShortName = &Config()->m_SvMap[i+1];
	}
	return pMapShortName;
}