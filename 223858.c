static inline rsRetVal tdlAdd(qqueue_t *pQueue, qDeqID deqID, int nElemDeq)
{
	toDeleteLst_t *pNew;
	toDeleteLst_t *pPrev;
	DEFiRet;

	ISOBJ_TYPE_assert(pQueue, qqueue);
	assert(pQueue->toDeleteLst != NULL);

	CHKmalloc(pNew = MALLOC(sizeof(toDeleteLst_t)));
	pNew->deqID = deqID;
	pNew->nElemDeq = nElemDeq;

	/* now find right spot */
	for(  pPrev = pQueue->toDeleteLst
	    ; pPrev != NULL && deqID > pPrev->deqID
	    ; pPrev = pPrev->pNext) {
		/*JUST SEARCH*/;
	}

	if(pPrev == NULL) {
		pNew->pNext = pQueue->toDeleteLst;
		pQueue->toDeleteLst = pNew;
	} else {
		pNew->pNext = pPrev->pNext;
		pPrev->pNext = pNew;
	}

finalize_it:
	RETiRet;
}