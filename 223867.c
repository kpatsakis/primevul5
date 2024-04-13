DeleteBatchFromQStore(qqueue_t *pThis, batch_t *pBatch)
{
	toDeleteLst_t *pTdl;
	qDeqID	deqIDDel;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	assert(pBatch != NULL);

	pTdl = tdlPeek(pThis); /* get current head element */
	if(pTdl == NULL) { /* to-delete list empty */
		DoDeleteBatchFromQStore(pThis, pBatch->nElem);
	} else if(pBatch->deqID == pThis->deqIDDel) {
		deqIDDel = pThis->deqIDDel;
		pTdl = tdlPeek(pThis);
		while(pTdl != NULL && deqIDDel == pTdl->deqID) {
			DoDeleteBatchFromQStore(pThis, pTdl->nElemDeq);
			tdlPop(pThis);
			++deqIDDel;
			pTdl = tdlPeek(pThis);
		}
		/* old entries deleted, now delete current ones... */
		DoDeleteBatchFromQStore(pThis, pBatch->nElem);
	} else {
		/* can not delete, insert into to-delete list */
		dbgprintf("not at head of to-delete list, enqueue %d\n", (int) pBatch->deqID);
		CHKiRet(tdlAdd(pThis, pBatch->deqID, pBatch->nElem));
	}

finalize_it:
	RETiRet;
}