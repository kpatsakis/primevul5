DataRcvdUncompressed(ptcpsess_t *pThis, char *pData, size_t iLen, struct syslogTime *stTime, time_t ttGenTime)
{
	multi_submit_t multiSub;
	smsg_t *pMsgs[CONF_NUM_MULTISUB];
	char *pEnd;
	unsigned nMsgs = 0;
	DEFiRet;

	assert(pData != NULL);
	assert(iLen > 0);

	if(ttGenTime == 0)
		datetime.getCurrTime(stTime, &ttGenTime, TIME_IN_LOCALTIME);
	multiSub.ppMsgs = pMsgs;
	multiSub.maxElem = CONF_NUM_MULTISUB;
	multiSub.nElem = 0;

	 /* We now copy the message to the session buffer. */
	pEnd = pData + iLen; /* this is one off, which is intensional */

	while(pData < pEnd) {
		CHKiRet(processDataRcvd(pThis, &pData, pEnd - pData, stTime, ttGenTime, &multiSub, &nMsgs));
		pData++;
	}

	iRet = multiSubmitFlush(&multiSub);

	if(glblSenderKeepTrack)
		statsRecordSender(propGetSzStr(pThis->peerName), nMsgs, ttGenTime);

finalize_it:
	RETiRet;
}