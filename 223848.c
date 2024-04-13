static rsRetVal qAddFixedArray(qqueue_t *pThis, void* in)
{
	DEFiRet;

	ASSERT(pThis != NULL);
	pThis->tVars.farray.pBuf[pThis->tVars.farray.tail] = in;
	pThis->tVars.farray.tail++;
	if (pThis->tVars.farray.tail == pThis->iMaxQueueSize)
		pThis->tVars.farray.tail = 0;

	RETiRet;
}