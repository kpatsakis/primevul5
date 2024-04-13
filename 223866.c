tryShutdownWorkersWithinQueueTimeout(qqueue_t *pThis)
{
	struct timespec tTimeout;
	rsRetVal iRetLocal;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ASSERT(pThis->pqParent == NULL); /* detect invalid calling sequence */

	if(pThis->bIsDA) {
		/* We need to lock the mutex, as otherwise we may have a race that prevents
		 * us from awaking the DA worker. */
		d_pthread_mutex_lock(pThis->mut);

		/* tell regular queue DA worker to stop shuffling messages to DA queue... */
		DBGOPRINT((obj_t*) pThis, "setting EnqOnly mode for DA worker\n");
		pThis->pqDA->bEnqOnly = 1;
		wtpSetState(pThis->pWtpDA, wtpState_SHUTDOWN_IMMEDIATE);
		wtpAdviseMaxWorkers(pThis->pWtpDA, 1);
		DBGOPRINT((obj_t*) pThis, "awoke DA worker, told it to shut down.\n");

		/* also tell the DA queue worker to shut down, so that it already knows... */
		wtpSetState(pThis->pqDA->pWtpReg, wtpState_SHUTDOWN);
		wtpAdviseMaxWorkers(pThis->pqDA->pWtpReg, 1); /* awake its lone worker */
		DBGOPRINT((obj_t*) pThis, "awoke DA queue regular worker, told it to shut down when done.\n");

		d_pthread_mutex_unlock(pThis->mut);
	}


	/* first calculate absolute timeout - we need the absolute value here, because we need to coordinate
	 * shutdown of both the regular and DA queue on *the same* timeout.
	 */
	timeoutComp(&tTimeout, pThis->toQShutdown);
	DBGOPRINT((obj_t*) pThis, "trying shutdown of regular workers\n");
	iRetLocal = wtpShutdownAll(pThis->pWtpReg, wtpState_SHUTDOWN, &tTimeout);
	if(iRetLocal == RS_RET_TIMED_OUT) {
		DBGOPRINT((obj_t*) pThis, "regular shutdown timed out on primary queue (this is OK)\n");
	} else {
		DBGOPRINT((obj_t*) pThis, "regular queue workers shut down.\n");
	}

	/* OK, the worker for the regular queue is processed, on the the DA queue regular worker. */
	if(pThis->pqDA != NULL) {
		DBGOPRINT((obj_t*) pThis, "we have a DA queue (0x%lx), requesting its shutdown.\n",
			 qqueueGetID(pThis->pqDA));
		/* we use the same absolute timeout as above, so we do not use more than the configured
		 * timeout interval!
		 */
		DBGOPRINT((obj_t*) pThis, "trying shutdown of regular worker of DA queue\n");
		iRetLocal = wtpShutdownAll(pThis->pqDA->pWtpReg, wtpState_SHUTDOWN, &tTimeout);
		if(iRetLocal == RS_RET_TIMED_OUT) {
			DBGOPRINT((obj_t*) pThis, "shutdown timed out on DA queue worker (this is OK)\n");
		} else {
			DBGOPRINT((obj_t*) pThis, "DA queue worker shut down.\n");
		}
	}

	RETiRet;
}