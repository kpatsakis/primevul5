static rsRetVal qDestructFixedArray(qqueue_t *pThis)
{
	DEFiRet;
	
	ASSERT(pThis != NULL);

	queueDrain(pThis); /* discard any remaining queue entries */
	free(pThis->tVars.farray.pBuf);

	RETiRet;
}