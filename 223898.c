DeleteProcessedBatch(qqueue_t *pThis, batch_t *pBatch)
{
	int i;
	void *pUsr;
	int nEnqueued = 0;
	rsRetVal localRet;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	assert(pBatch != NULL);

	for(i = 0 ; i < pBatch->nElem ; ++i) {
		pUsr = pBatch->pElem[i].pUsrp;
		if(   pBatch->pElem[i].state == BATCH_STATE_RDY
		   || pBatch->pElem[i].state == BATCH_STATE_SUB) {
dbgprintf("XXX: DeleteProcessedBatch re-enqueue %d of %d, state %d\n", i, pBatch->nElem, pBatch->pElem[i].state);
			localRet = doEnqSingleObj(pThis, eFLOWCTL_NO_DELAY,
				       (obj_t*)MsgAddRef((msg_t*) pUsr));
			++nEnqueued;
			if(localRet != RS_RET_OK) {
				DBGPRINTF("error %d re-enqueuing unprocessed data element - discarded\n", localRet);
			}
		}
		objDestruct(pUsr);
	}

	dbgprintf("we deleted %d objects and enqueued %d objects\n", i-nEnqueued, nEnqueued);

	if(nEnqueued > 0)
		qqueueChkPersist(pThis, nEnqueued);

	iRet = DeleteBatchFromQStore(pThis, pBatch);

	pBatch->nElem = pBatch->nElemDeq = 0; /* reset batch */ // TODO: more fine init, new fields! 2010-06-14

	RETiRet;
}