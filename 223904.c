qqueueChkStopWrkrDA(qqueue_t *pThis)
{
	DEFiRet;

	if(pThis->bEnqOnly) {
		iRet = RS_RET_TERMINATE_WHEN_IDLE;
	}

	RETiRet;
}