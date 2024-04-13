batchProcessed(qqueue_t *pThis, wti_t *pWti)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ISOBJ_TYPE_assert(pWti, wti);

	int iCancelStateSave;
	/* at this spot, we must not be cancelled */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &iCancelStateSave);
	DeleteProcessedBatch(pThis, &pWti->batch);
	qqueueChkPersist(pThis, pWti->batch.nElemDeq);
	pthread_setcancelstate(iCancelStateSave, NULL);

	RETiRet;
}