static rsRetVal qDelFixedArray(qqueue_t *pThis)
{
	DEFiRet;

	ASSERT(pThis != NULL);

	pThis->tVars.farray.head++;
	if (pThis->tVars.farray.head == pThis->iMaxQueueSize)
		pThis->tVars.farray.head = 0;

	RETiRet;
}