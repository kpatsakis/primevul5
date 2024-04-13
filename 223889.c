DoSaveOnShutdown(qqueue_t *pThis)
{
	struct timespec tTimeout;
	rsRetVal iRetLocal;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);

	/* we reduce the low water mark, otherwise the DA worker would terminate when
	 * it is reached.
	 */
	DBGOPRINT((obj_t*) pThis, "bSaveOnShutdown set, restarting DA worker...\n");
	pThis->bShutdownImmediate = 0; /* would termiante the DA worker! */
	pThis->iLowWtrMrk = 0;
	wtpSetState(pThis->pWtpDA, wtpState_SHUTDOWN);	/* shutdown worker (only) when done (was _IMMEDIATE!) */
	wtpAdviseMaxWorkers(pThis->pWtpDA, 1);		/* restart DA worker */

	DBGOPRINT((obj_t*) pThis, "waiting for DA worker to terminate...\n");
	timeoutComp(&tTimeout, QUEUE_TIMEOUT_ETERNAL);
	/* and run the primary queue's DA worker to drain the queue */
	iRetLocal = wtpShutdownAll(pThis->pWtpDA, wtpState_SHUTDOWN, &tTimeout);
	DBGOPRINT((obj_t*) pThis, "end queue persistence run, iRet %d, queue size log %d, phys %d\n",
		  iRetLocal, getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis));
	if(iRetLocal != RS_RET_OK) {
		DBGOPRINT((obj_t*) pThis, "unexpected iRet state %d after trying to shut down primary queue in disk save mode, "
			  "continuing, but results are unpredictable\n", iRetLocal);
	}

	RETiRet;
}