startupServers(void)
{
	DEFiRet;
	rsRetVal localRet, lastErr;
	int iOK;
	int iAll;
	ptcpsrv_t *pSrv;

	iAll = iOK = 0;
	lastErr = RS_RET_ERR;
	pSrv = pSrvRoot;
	while(pSrv != NULL) {
		DBGPRINTF("imptcp: starting up server for port %s, name '%s'\n", pSrv->port, pSrv->pszInputName);
		localRet = startupSrv(pSrv);
		if(localRet == RS_RET_OK)
			iOK++;
		else
			lastErr = localRet;
		++iAll;
		pSrv = pSrv->pNext;
	}

	DBGPRINTF("imptcp: %d out of %d servers started successfully\n", iOK, iAll);
	if(iOK == 0)	/* iff all fails, we report an error */
		iRet = lastErr;
		
	RETiRet;
}