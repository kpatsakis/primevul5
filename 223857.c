cancelWorkers(qqueue_t *pThis)
{
	rsRetVal iRetLocal;
	DEFiRet;

	/* Now queue workers should have terminated. If not, we need to cancel them as we have applied
	 * all timeout setting. If any worker in any queue still executes, its consumer is possibly
	 * long-running and cancelling is the only way to get rid of it.
	 */
	DBGOPRINT((obj_t*) pThis, "checking to see if we need to cancel any worker threads of the primary queue\n");
	iRetLocal = wtpCancelAll(pThis->pWtpReg); /* returns immediately if all threads already have terminated */
	if(iRetLocal != RS_RET_OK) {
		DBGOPRINT((obj_t*) pThis, "unexpected iRet state %d trying to cancel primary queue worker "
			  "threads, continuing, but results are unpredictable\n", iRetLocal);
	}

	/* ... and now the DA queue, if it exists (should always be after the primary one) */
	if(pThis->pqDA != NULL) {
		DBGOPRINT((obj_t*) pThis, "checking to see if we need to cancel any worker threads of the DA queue\n");
		iRetLocal = wtpCancelAll(pThis->pqDA->pWtpReg); /* returns immediately if all threads already have terminated */
		if(iRetLocal != RS_RET_OK) {
			DBGOPRINT((obj_t*) pThis, "unexpected iRet state %d trying to cancel DA queue worker "
				  "threads, continuing, but results are unpredictable\n", iRetLocal);
		}

		/* finally, we cancel the main queue's DA worker pool, if it still is running. It may be
		 * restarted later to persist the queue. But we stop it, because otherwise we get into
		 * big trouble when resetting the logical dequeue pointer. This operation can only be
		 * done when *no* worker is running. So time for a shutdown... -- rgerhards, 2009-05-28
		 */
		DBGOPRINT((obj_t*) pThis, "checking to see if main queue DA worker pool needs to be cancelled\n");
		wtpCancelAll(pThis->pWtpDA); /* returns immediately if all threads already have terminated */
	}

	RETiRet;
}