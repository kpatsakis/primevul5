doSubmitMsg(ptcpsess_t *pThis, struct syslogTime *stTime, time_t ttGenTime, multi_submit_t *pMultiSub)
{
	smsg_t *pMsg;
	ptcpsrv_t *pSrv;
	DEFiRet;

	if(pThis->iMsg == 0) {
		DBGPRINTF("discarding zero-sized message\n");
		FINALIZE;
	}
	pSrv = pThis->pLstn->pSrv;

	/* we now create our own message object and submit it to the queue */
	CHKiRet(msgConstructWithTime(&pMsg, stTime, ttGenTime));
	MsgSetRawMsg(pMsg, (char*)pThis->pMsg, pThis->iMsg);
	MsgSetInputName(pMsg, pSrv->pInputName);
	MsgSetFlowControlType(pMsg, eFLOWCTL_LIGHT_DELAY);
	if(pSrv->dfltTZ != NULL)
		MsgSetDfltTZ(pMsg, (char*) pSrv->dfltTZ);
	pMsg->msgFlags  = NEEDS_PARSING | PARSE_HOSTNAME;
	MsgSetRcvFrom(pMsg, pThis->peerName);
	CHKiRet(MsgSetRcvFromIP(pMsg, pThis->peerIP));
	MsgSetRuleset(pMsg, pSrv->pRuleset);
	STATSCOUNTER_INC(pThis->pLstn->ctrSubmit, pThis->pLstn->mutCtrSubmit);

	ratelimitAddMsg(pSrv->ratelimiter, pMultiSub, pMsg);

finalize_it:
	/* reset status variables */
	pThis->bAtStrtOfFram = 1;
	pThis->iMsg = 0;

	RETiRet;
}