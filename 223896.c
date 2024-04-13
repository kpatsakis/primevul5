static rsRetVal qDeqFixedArray(qqueue_t *pThis, void **out)
{
	DEFiRet;

	ASSERT(pThis != NULL);
	*out = (void*) pThis->tVars.farray.pBuf[pThis->tVars.farray.deqhead];

	pThis->tVars.farray.deqhead++;
	if (pThis->tVars.farray.deqhead == pThis->iMaxQueueSize)
		pThis->tVars.farray.deqhead = 0;

	RETiRet;
}