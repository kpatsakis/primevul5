static rsRetVal qqueueChkPersist(qqueue_t *pThis, int nUpdates)
{
	DEFiRet;
	ISOBJ_TYPE_assert(pThis, qqueue);
	assert(nUpdates >= 0);

	if(nUpdates == 0)
		FINALIZE;

	pThis->iUpdsSincePersist += nUpdates;
	if(pThis->iPersistUpdCnt && pThis->iUpdsSincePersist >= pThis->iPersistUpdCnt) {
		qqueuePersist(pThis, QUEUE_CHECKPOINT);
		pThis->iUpdsSincePersist = 0;
	}

finalize_it:
	RETiRet;
}