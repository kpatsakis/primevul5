static rsRetVal qConstructFixedArray(qqueue_t *pThis)
{
	DEFiRet;

	ASSERT(pThis != NULL);

	if(pThis->iMaxQueueSize == 0)
		ABORT_FINALIZE(RS_RET_QSIZE_ZERO);

	if((pThis->tVars.farray.pBuf = MALLOC(sizeof(void *) * pThis->iMaxQueueSize)) == NULL) {
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);
	}

	pThis->tVars.farray.deqhead = 0;
	pThis->tVars.farray.head = 0;
	pThis->tVars.farray.tail = 0;

	qqueueChkIsDA(pThis);

finalize_it:
	RETiRet;
}