addLstn(ptcpsrv_t *pSrv, int sock, int isIPv6)
{
	DEFiRet;
	ptcplstn_t *pLstn = NULL;
	uchar statname[64];

	CHKmalloc(pLstn = calloc(1, sizeof(ptcplstn_t)));
	pLstn->pSrv = pSrv;
	pLstn->bSuppOctetFram = pSrv->bSuppOctetFram;
	pLstn->bSPFramingFix = pSrv->bSPFramingFix;
	pLstn->sock = sock;
	/* support statistics gathering */
	uchar *inputname;
	if(pSrv->pszInputName == NULL) {
		inputname = (uchar*)"imptcp";
	} else {
		inputname = pSrv->pszInputName;
	}
	CHKiRet(statsobj.Construct(&(pLstn->stats)));
	snprintf((char*)statname, sizeof(statname), "%s(%s/%s/%s)", inputname,
		(pSrv->lstnIP == NULL) ? "*" : (char*)pSrv->lstnIP, pSrv->port,
		isIPv6 ? "IPv6" : "IPv4");
	statname[sizeof(statname)-1] = '\0'; /* just to be on the save side... */
	CHKiRet(statsobj.SetName(pLstn->stats, statname));
	CHKiRet(statsobj.SetOrigin(pLstn->stats, (uchar*)"imptcp"));
	STATSCOUNTER_INIT(pLstn->ctrSubmit, pLstn->mutCtrSubmit);
	CHKiRet(statsobj.AddCounter(pLstn->stats, UCHAR_CONSTANT("submitted"),
		ctrType_IntCtr, CTR_FLAG_RESETTABLE, &(pLstn->ctrSubmit)));
	/* the following counters are not protected by mutexes; we accept
	 * that they may not be 100% correct */
	pLstn->rcvdBytes = 0,
	pLstn->rcvdDecompressed = 0;
	CHKiRet(statsobj.AddCounter(pLstn->stats, UCHAR_CONSTANT("bytes.received"),
		ctrType_IntCtr, CTR_FLAG_RESETTABLE, &(pLstn->rcvdBytes)));
	CHKiRet(statsobj.AddCounter(pLstn->stats, UCHAR_CONSTANT("bytes.decompressed"),
		ctrType_IntCtr, CTR_FLAG_RESETTABLE, &(pLstn->rcvdDecompressed)));
	CHKiRet(statsobj.ConstructFinalize(pLstn->stats));

	CHKiRet(addEPollSock(epolld_lstn, pLstn, sock, &pLstn->epd));

	/* add to start of server's listener list */
	pLstn->prev = NULL;
	pLstn->next = pSrv->pLstn;
	if(pSrv->pLstn != NULL)
		pSrv->pLstn->prev = pLstn;
	pSrv->pLstn = pLstn;

finalize_it:
	if(iRet != RS_RET_OK) {
		if(pLstn != NULL) {
			if(pLstn->stats != NULL)
				statsobj.Destruct(&(pLstn->stats));
			free(pLstn);
		}
	}

	RETiRet;
}