static inline rsRetVal tdlPop(qqueue_t *pQueue)
{
	toDeleteLst_t *pRemove;
	DEFiRet;

	ISOBJ_TYPE_assert(pQueue, qqueue);
	assert(pQueue->toDeleteLst != NULL);

	pRemove = pQueue->toDeleteLst;
	pQueue->toDeleteLst = pQueue->toDeleteLst->pNext;
	free(pRemove);

	RETiRet;
}