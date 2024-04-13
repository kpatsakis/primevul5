ConsumerReg(qqueue_t *pThis, wti_t *pWti)
{
	int iCancelStateSave;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ISOBJ_TYPE_assert(pWti, wti);

	CHKiRet(DequeueForConsumer(pThis, pWti));

	/* we now have a non-idle batch of work, so we can release the queue mutex and process it */
	d_pthread_mutex_unlock(pThis->mut);

	/* at this spot, we may be cancelled */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &iCancelStateSave);

	CHKiRet(pThis->pConsumer(pThis->pUsr, &pWti->batch, &pThis->bShutdownImmediate));

	/* we now need to check if we should deliberately delay processing a bit
	 * and, if so, do that. -- rgerhards, 2008-01-30
	 */
//TODO: MULTIQUEUE: the following setting is no longer correct - need to think about how to do that...
	if(pThis->iDeqSlowdown) {
		DBGOPRINT((obj_t*) pThis, "sleeping %d microseconds as requested by config params\n",
			  pThis->iDeqSlowdown);
		srSleep(pThis->iDeqSlowdown / 1000000, pThis->iDeqSlowdown % 1000000);
	}

	/* but now cancellation is no longer permitted */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &iCancelStateSave);

	/* now we are done, but need to re-aquire the mutex */
	d_pthread_mutex_lock(pThis->mut);

finalize_it:
	dbgprintf("regular consumer finished, iret=%d, szlog %d sz phys %d\n", iRet,
	          getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis));
	RETiRet;
}