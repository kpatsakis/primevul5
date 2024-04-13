static rsRetVal addInstance(void __attribute__((unused)) *pVal, uchar *pNewVal)
{
	instanceConf_t *inst;
	DEFiRet;

	CHKiRet(createInstance(&inst));
	if(pNewVal == NULL || *pNewVal == '\0') {
		errmsg.LogError(0, NO_ERRCODE, "imptcp: port number must be specified, listener ignored");
	}
	if((pNewVal == NULL) || (pNewVal == '\0')) {
		inst->pszBindPort = NULL;
	} else {
		CHKmalloc(inst->pszBindPort = ustrdup(pNewVal));
	}
	if((cs.lstnIP == NULL) || (cs.lstnIP[0] == '\0')) {
		inst->pszBindAddr = NULL;
	} else {
		CHKmalloc(inst->pszBindAddr = ustrdup(cs.lstnIP));
	}
	if((cs.pszBindRuleset == NULL) || (cs.pszBindRuleset[0] == '\0')) {
		inst->pszBindRuleset = NULL;
	} else {
		CHKmalloc(inst->pszBindRuleset = ustrdup(cs.pszBindRuleset));
	}
	if((cs.pszInputName == NULL) || (cs.pszInputName[0] == '\0')) {
		inst->pszInputName = NULL;
	} else {
		CHKmalloc(inst->pszInputName = ustrdup(cs.pszInputName));
	}
	inst->pBindRuleset = NULL;
	inst->bSuppOctetFram = cs.bSuppOctetFram;
	inst->bKeepAlive = cs.bKeepAlive;
	inst->iKeepAliveIntvl = cs.iKeepAliveTime;
	inst->iKeepAliveProbes = cs.iKeepAliveProbes;
	inst->iKeepAliveTime = cs.iKeepAliveTime;
	inst->bEmitMsgOnClose = cs.bEmitMsgOnClose;
	inst->iAddtlFrameDelim = cs.iAddtlFrameDelim;

finalize_it:
	free(pNewVal);
	RETiRet;
}