tryShutdownWorkersWithinActionTimeout(qqueue_t *pThis)
{
	struct timespec tTimeout;
	rsRetVal iRetLocal;
	DEFiRet;

RUNLOG_STR("trying to shutdown workers within Action Timeout");
	ISOBJ_TYPE_assert(pThis, qqueue);
	ASSERT(pThis->pqParent == NULL); /* detect invalid calling sequence */

	/* instruct workers to finish ASAP, even if still work exists */
	DBGOPRINT((obj_t*) pThis, "setting EnqOnly mode\n");
	pThis->bEnqOnly = 1;
	pThis->bShutdownImmediate = 1;
	/* now DA queue */
	if(pThis->bIsDA) {
		pThis->pqDA->bEnqOnly = 1;
		pThis->pqDA->bShutdownImmediate = 1;
	}

// TODO: make sure we have at minimum a 10ms timeout - workers deserve a chance...
	/* now give the queue workers a last chance to gracefully shut down (based on action timeout setting) */
	timeoutComp(&tTimeout, pThis->toActShutdown);
	DBGOPRINT((obj_t*) pThis, "trying immediate shutdown of regular workers (if any)\n");
	iRetLocal = wtpShutdownAll(pThis->pWtpReg, wtpState_SHUTDOWN_IMMEDIATE, &tTimeout);
	if(iRetLocal == RS_RET_TIMED_OUT) {
		DBGOPRINT((obj_t*) pThis, "immediate shutdown timed out on primary queue (this is acceptable and "
			  "triggers cancellation)\n");
	} else if(iRetLocal != RS_RET_OK) {
		DBGOPRINT((obj_t*) pThis, "unexpected iRet state %d after trying immediate shutdown of the primary queue "
			  "in disk save mode. Continuing, but results are unpredictable\n", iRetLocal);
	}

	if(pThis->pqDA != NULL) {
		/* and now the same for the DA queue */
		DBGOPRINT((obj_t*) pThis, "trying immediate shutdown of DA queue workers\n");
		iRetLocal = wtpShutdownAll(pThis->pqDA->pWtpReg, wtpState_SHUTDOWN_IMMEDIATE, &tTimeout);
		if(iRetLocal == RS_RET_TIMED_OUT) {
			DBGOPRINT((obj_t*) pThis, "immediate shutdown timed out on DA queue (this is acceptable "
				  "and triggers cancellation)\n");
		} else if(iRetLocal != RS_RET_OK) {
			DBGOPRINT((obj_t*) pThis, "unexpected iRet state %d after trying immediate shutdown of the DA "
				  "queue in disk save mode. Continuing, but results are unpredictable\n", iRetLocal);
		}

		/* and now we need to terminate the DA worker itself. We always grant it a 100ms timeout,
		 * which should be sufficient and usually not be required (it is expected to have finished
		 * long before while we were processing the queue timeout in shutdown phase 1).
		 * rgerhards, 2009-10-14
		 */
		timeoutComp(&tTimeout, 100);
		DBGOPRINT((obj_t*) pThis, "trying regular shutdown of main queue DA worker pool\n");
		iRetLocal = wtpShutdownAll(pThis->pWtpDA, wtpState_SHUTDOWN_IMMEDIATE, &tTimeout);
		if(iRetLocal == RS_RET_TIMED_OUT) {
			DBGOPRINT((obj_t*) pThis, "shutdown timed out on main queue DA worker pool "
					          "(this is not good, but probably OK)\n");
		} else {
			DBGOPRINT((obj_t*) pThis, "main queue DA worker pool shut down.\n");
		}
	}

	RETiRet;
}