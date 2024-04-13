addListner(modConfData_t __attribute__((unused)) *modConf, instanceConf_t *inst)
{
	DEFiRet;
	ptcpsrv_t *pSrv = NULL;

	CHKmalloc(pSrv = calloc(1, sizeof(ptcpsrv_t)));
	pthread_mutex_init(&pSrv->mutSessLst, NULL);
	pSrv->pSess = NULL;
	pSrv->pLstn = NULL;
	pSrv->bSuppOctetFram = inst->bSuppOctetFram;
	pSrv->bSPFramingFix = inst->bSPFramingFix;
	pSrv->bKeepAlive = inst->bKeepAlive;
	pSrv->iKeepAliveIntvl = inst->iKeepAliveTime;
	pSrv->iKeepAliveProbes = inst->iKeepAliveProbes;
	pSrv->iKeepAliveTime = inst->iKeepAliveTime;
	pSrv->bEmitMsgOnClose = inst->bEmitMsgOnClose;
	pSrv->compressionMode = inst->compressionMode;
	pSrv->dfltTZ = inst->dfltTZ;
	if (inst->pszBindPort != NULL) {
		CHKmalloc(pSrv->port = ustrdup(inst->pszBindPort));
	}
	pSrv->iAddtlFrameDelim = inst->iAddtlFrameDelim;
	if (inst->pszBindAddr == NULL) {
		pSrv->lstnIP = NULL;
	} else {
		CHKmalloc(pSrv->lstnIP = ustrdup(inst->pszBindAddr));
	}
	if (inst->pszBindPath == NULL) {
		pSrv->path = NULL;
	} else {
		CHKmalloc(pSrv->path = ustrdup(inst->pszBindPath));
		CHKmalloc(pSrv->port = ustrdup(inst->pszBindPath));
		pSrv->bUnixSocket = 1;
		pSrv->fCreateMode = inst->fCreateMode;
		pSrv->fileUID = inst->fileUID;
		pSrv->fileGID = inst->fileGID;
		pSrv->bFailOnPerms = inst->bFailOnPerms;
	}

	pSrv->bUnlink = inst->bUnlink;
	pSrv->pRuleset = inst->pBindRuleset;
	pSrv->pszInputName = ustrdup((inst->pszInputName == NULL) ?  UCHAR_CONSTANT("imptcp") : inst->pszInputName);
	CHKiRet(prop.Construct(&pSrv->pInputName));
	CHKiRet(prop.SetString(pSrv->pInputName, pSrv->pszInputName, ustrlen(pSrv->pszInputName)));
	CHKiRet(prop.ConstructFinalize(pSrv->pInputName));

	CHKiRet(ratelimitNew(&pSrv->ratelimiter, "imptcp", (char*) pSrv->port));
	ratelimitSetLinuxLike(pSrv->ratelimiter, inst->ratelimitInterval, inst->ratelimitBurst);
	ratelimitSetThreadSafe(pSrv->ratelimiter);
	/* add to linked list */
	pSrv->pNext = pSrvRoot;
	pSrvRoot = pSrv;

	/* all config vars are auto-reset -- this also is very useful with the
	 * new config format effort (v6).
	 */
	resetConfigVariables(NULL, NULL);

finalize_it:
	if(iRet != RS_RET_OK) {
		errmsg.LogError(0, NO_ERRCODE, "error %d trying to add listener", iRet);
		if(pSrv != NULL) {
			destructSrv(pSrv);
		}
	}
	RETiRet;
}