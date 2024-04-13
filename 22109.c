addSess(ptcplstn_t *pLstn, int sock, prop_t *peerName, prop_t *peerIP)
{
	DEFiRet;
	ptcpsess_t *pSess = NULL;
	ptcpsrv_t *pSrv = pLstn->pSrv;

	CHKmalloc(pSess = malloc(sizeof(ptcpsess_t)));
	CHKmalloc(pSess->pMsg = malloc(iMaxLine));
	pSess->pLstn = pLstn;
	pSess->sock = sock;
	pSess->bSuppOctetFram = pLstn->bSuppOctetFram;
	pSess->bSPFramingFix = pLstn->bSPFramingFix;
	pSess->inputState = eAtStrtFram;
	pSess->iMsg = 0;
	pSess->bzInitDone = 0;
	pSess->bAtStrtOfFram = 1;
	pSess->peerName = peerName;
	pSess->peerIP = peerIP;
	pSess->compressionMode = pLstn->pSrv->compressionMode;

	/* add to start of server's listener list */
	pSess->prev = NULL;
	pthread_mutex_lock(&pSrv->mutSessLst);
	pSess->next = pSrv->pSess;
	if(pSrv->pSess != NULL)
		pSrv->pSess->prev = pSess;
	pSrv->pSess = pSess;
	pthread_mutex_unlock(&pSrv->mutSessLst);

	CHKiRet(addEPollSock(epolld_sess, pSess, sock, &pSess->epd));

finalize_it:
	if(iRet != RS_RET_OK) {
		if(pSess != NULL) {
			if(pSess->pMsg != NULL)
				free(pSess->pMsg);
			free(pSess);
		}
	}

	RETiRet;
}