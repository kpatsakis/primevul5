static inline toDeleteLst_t *tdlPeek(qqueue_t *pQueue)
{
	ISOBJ_TYPE_assert(pQueue, qqueue);
	return pQueue->toDeleteLst;
}