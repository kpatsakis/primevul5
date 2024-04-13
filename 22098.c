resetConfigVariables(uchar __attribute__((unused)) *pp, void __attribute__((unused)) *pVal)
{
	cs.bEmitMsgOnClose = 0;
	cs.wrkrMax = DFLT_wrkrMax;
	cs.bKeepAlive = 0;
	cs.iKeepAliveProbes = 0;
	cs.iKeepAliveTime = 0;
	cs.iKeepAliveIntvl = 0;
	cs.bSuppOctetFram = 1;
	cs.iAddtlFrameDelim = TCPSRV_NO_ADDTL_DELIMITER;
	free(cs.pszInputName);
	cs.pszInputName = NULL;
	free(cs.lstnIP);
	cs.lstnIP = NULL;
	return RS_RET_OK;
}