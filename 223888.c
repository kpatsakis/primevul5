qqueueEnqObj(qqueue_t *pThis, flowControl_t flowCtlType, void *pUsr)
{
	DEFiRet;
	int iCancelStateSave;

	ISOBJ_TYPE_assert(pThis, qqueue);

	if(pThis->qType != QUEUETYPE_DIRECT) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &iCancelStateSave);
		d_pthread_mutex_lock(pThis->mut);
	}

	CHKiRet(doEnqSingleObj(pThis, flowCtlType, pUsr));

	qqueueChkPersist(pThis, 1);

finalize_it:
	if(pThis->qType != QUEUETYPE_DIRECT) {
		/* make sure at least one worker is running. */
		qqueueAdviseMaxWorkers(pThis);
		/* and release the mutex */
		d_pthread_mutex_unlock(pThis->mut);
		pthread_setcancelstate(iCancelStateSave, NULL);
		DBGOPRINT((obj_t*) pThis, "EnqueueMsg advised worker start\n");
	}

	RETiRet;
}