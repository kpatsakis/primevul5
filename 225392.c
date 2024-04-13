int CServer::MapListEntryCallback(const char *pFilename, int IsDir, int DirType, void *pUser)
{
	CSubdirCallbackUserdata *pUserdata = (CSubdirCallbackUserdata *)pUser;
	CServer *pThis = pUserdata->m_pServer;

	if(pFilename[0] == '.') // hidden files
		return 0;

	char aFilename[IO_MAX_PATH_LENGTH];
	if(pUserdata->m_aName[0])
		str_format(aFilename, sizeof(aFilename), "%s/%s", pUserdata->m_aName, pFilename);
	else
		str_format(aFilename, sizeof(aFilename), "%s", pFilename);

	if(IsDir)
	{
		CSubdirCallbackUserdata Userdata;
		Userdata.m_pServer = pThis;
		str_copy(Userdata.m_aName, aFilename, sizeof(Userdata.m_aName));
		char FindPath[IO_MAX_PATH_LENGTH];
		str_format(FindPath, sizeof(FindPath), "maps/%s/", aFilename);
		pThis->m_pStorage->ListDirectory(IStorage::TYPE_ALL, FindPath, MapListEntryCallback, &Userdata);
		return 0;
	}

	const char *pSuffix = str_endswith(aFilename, ".map");
	if(!pSuffix) // not ending with .map
	{
			return 0;
	}

	CMapListEntry *pEntry = (CMapListEntry *)pThis->m_pMapListHeap->Allocate(sizeof(CMapListEntry));
	pThis->m_NumMapEntries++;
	pEntry->m_pNext = 0;
	pEntry->m_pPrev = pThis->m_pLastMapEntry;
	if(pEntry->m_pPrev)
		pEntry->m_pPrev->m_pNext = pEntry;
	pThis->m_pLastMapEntry = pEntry;
	if(!pThis->m_pFirstMapEntry)
		pThis->m_pFirstMapEntry = pEntry;

	str_truncate(pEntry->m_aName, sizeof(pEntry->m_aName), aFilename, pSuffix-aFilename);

	return 0;
}