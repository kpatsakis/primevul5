qqueueDeq(qqueue_t *pThis, void **ppUsr)
{
	DEFiRet;

	ASSERT(pThis != NULL);

	/* we do NOT abort if we encounter an error, because otherwise the queue
	 * will not be decremented, what will most probably result in an endless loop.
	 * If we decrement, however, we may lose a message. But that is better than
	 * losing the whole process because it loops... -- rgerhards, 2008-01-03
	 */
	iRet = pThis->qDeq(pThis, ppUsr);
	ATOMIC_INC(&pThis->nLogDeq, &pThis->mutLogDeq);

//	DBGOPRINT((obj_t*) pThis, "entry deleted, size now log %d, phys %d entries\n",
//		  getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis));

	RETiRet;
}