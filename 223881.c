qqueueAdviseMaxWorkers(qqueue_t *pThis)
{
	DEFiRet;
	int iMaxWorkers;

	ISOBJ_TYPE_assert(pThis, qqueue);

	if(!pThis->bEnqOnly) {
		if(pThis->bIsDA && getLogicalQueueSize(pThis) >= pThis->iHighWtrMrk) {
			wtpAdviseMaxWorkers(pThis->pWtpDA, 1); /* disk queues have always one worker */
		} else {
			if(getLogicalQueueSize(pThis) == 0) {
				iMaxWorkers = 0;
			} else if(pThis->qType == QUEUETYPE_DISK || pThis->iMinMsgsPerWrkr == 0) {
				iMaxWorkers = 1;
			} else {
				iMaxWorkers = getLogicalQueueSize(pThis) / pThis->iMinMsgsPerWrkr + 1;
			}
			wtpAdviseMaxWorkers(pThis->pWtpReg, iMaxWorkers);
		}
	}

	RETiRet;
}