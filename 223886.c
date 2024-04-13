GetDeqBatchSize(qqueue_t *pThis, int *pVal)
{
	DEFiRet;
	assert(pVal != NULL);
	*pVal = pThis->iDeqBatchSize;
if(pThis->pqParent != NULL) // TODO: check why we actually do this!
	*pVal = 16;
	RETiRet;
}