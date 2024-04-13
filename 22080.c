createInstance(instanceConf_t **pinst)
{
	instanceConf_t *inst;
	DEFiRet;
	CHKmalloc(inst = MALLOC(sizeof(instanceConf_t)));
	inst->next = NULL;

	inst->pszBindPort = NULL;
	inst->pszBindAddr = NULL;
	inst->pszBindPath = NULL;
	inst->fileUID = -1;
	inst->fileGID = -1;
	inst->fCreateMode = 0644;
	inst->bFailOnPerms = 1;
	inst->bUnlink = 0;
	inst->pszBindRuleset = NULL;
	inst->pszInputName = NULL;
	inst->bSuppOctetFram = 1;
	inst->bSPFramingFix = 0;
	inst->bKeepAlive = 0;
	inst->iKeepAliveIntvl = 0;
	inst->iKeepAliveProbes = 0;
	inst->iKeepAliveTime = 0;
	inst->bEmitMsgOnClose = 0;
	inst->dfltTZ = NULL;
	inst->iAddtlFrameDelim = TCPSRV_NO_ADDTL_DELIMITER;
	inst->pBindRuleset = NULL;
	inst->ratelimitBurst = 10000; /* arbitrary high limit */
	inst->ratelimitInterval = 0; /* off */
	inst->compressionMode = COMPRESS_SINGLE_MSG;

	/* node created, let's add to config */
	if(loadModConf->tail == NULL) {
		loadModConf->tail = loadModConf->root = inst;
	} else {
		loadModConf->tail->next = inst;
		loadModConf->tail = inst;
	}

	*pinst = inst;
finalize_it:
	RETiRet;
}