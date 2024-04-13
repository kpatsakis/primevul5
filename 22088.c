closeSess(ptcpsess_t *pSess)
{
	int sock;
	DEFiRet;
	
	if(pSess->compressionMode >= COMPRESS_STREAM_ALWAYS)
		doZipFinish(pSess);

	sock = pSess->sock;
	CHKiRet(removeEPollSock(sock, pSess->epd));
	close(sock);

	pthread_mutex_lock(&pSess->pLstn->pSrv->mutSessLst);
	/* finally unlink session from structures */
	if(pSess->next != NULL)
		pSess->next->prev = pSess->prev;
	if(pSess->prev == NULL) {
		/* need to update root! */
		pSess->pLstn->pSrv->pSess = pSess->next;
	} else {
		pSess->prev->next = pSess->next;
	}
	pthread_mutex_unlock(&pSess->pLstn->pSrv->mutSessLst);

	/* unlinked, now remove structure */
	destructSess(pSess);

finalize_it:
	DBGPRINTF("imptcp: session on socket %d closed with iRet %d.\n", sock, iRet);
	RETiRet;
}