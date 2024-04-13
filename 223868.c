ConsumerDA(qqueue_t *pThis, wti_t *pWti)
{
	int i;
	int iCancelStateSave;
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ISOBJ_TYPE_assert(pWti, wti);

	CHKiRet(DequeueForConsumer(pThis, pWti));

	/* we now have a non-idle batch of work, so we can release the queue mutex and process it */
	d_pthread_mutex_unlock(pThis->mut);

	/* at this spot, we may be cancelled */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &iCancelStateSave);

	/* iterate over returned results and enqueue them in DA queue */
	for(i = 0 ; i < pWti->batch.nElem && !pThis->bShutdownImmediate ; i++) {
		/* TODO: we must add a generic "addRef" mechanism, because the disk queue enqueue destructs
		 * the message. So far, we simply assume we always have msg_t, what currently is always the case.
		 * rgerhards, 2009-05-28
		 */
		CHKiRet(qqueueEnqObj(pThis->pqDA, eFLOWCTL_NO_DELAY,
			(obj_t*)MsgAddRef((msg_t*)(pWti->batch.pElem[i].pUsrp))));
		pWti->batch.pElem[i].state = BATCH_STATE_COMM; /* commited to other queue! */
	}

	/* but now cancellation is no longer permitted */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &iCancelStateSave);

	/* now we are done, but need to re-aquire the mutex */
	d_pthread_mutex_lock(pThis->mut);

finalize_it:
	DBGOPRINT((obj_t*) pThis, "DAConsumer returns with iRet %d\n", iRet);
	RETiRet;
}