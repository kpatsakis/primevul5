DoDeleteBatchFromQStore(qqueue_t *pThis, int nElem)
{
	int i;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);

	/* now send delete request to storage driver */
	for(i = 0 ; i < nElem ; ++i) {
		pThis->qDel(pThis);
	}

	/* iQueueSize is not decremented by qDel(), so we need to do it ourselves */
	ATOMIC_SUB(&pThis->iQueueSize, nElem, &pThis->mutQueueSize);
	ATOMIC_SUB(&pThis->nLogDeq, nElem, &pThis->mutLogDeq);
dbgprintf("delete batch from store, new sizes: log %d, phys %d\n", getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis));
	++pThis->deqIDDel; /* one more batch dequeued */

	RETiRet;
}