DequeueConsumable(qqueue_t *pThis, wti_t *pWti)
{
	DEFiRet;
	int iQueueSize = 0; /* keep the compiler happy... */

	/* dequeue element batch (still protected from mutex) */
	iRet = DequeueConsumableElements(pThis, pWti, &iQueueSize);

	/* awake some flow-controlled sources if we can do this right now */
	/* TODO: this could be done better from a performance point of view -- do it only if
	 * we have someone waiting for the condition (or only when we hit the watermark right
	 * on the nail [exact value]) -- rgerhards, 2008-03-14
	 * now that we dequeue batches of pointers, this is much less an issue...
	 * rgerhards, 2009-04-22
	 */
	if(iQueueSize < pThis->iFullDlyMrk / 2) {
		pthread_cond_broadcast(&pThis->belowFullDlyWtrMrk);
	}

	if(iQueueSize < pThis->iLightDlyMrk / 2) {
		pthread_cond_broadcast(&pThis->belowLightDlyWtrMrk);
	}

	// TODO: MULTI: check physical queue size?
	pthread_cond_signal(&pThis->notFull);
	/* WE ARE NO LONGER PROTECTED BY THE MUTEX */

	if(iRet != RS_RET_OK && iRet != RS_RET_DISCARDMSG) {
		DBGOPRINT((obj_t*) pThis, "error %d dequeueing element - ignoring, but strange things "
			  "may happen\n", iRet);
	}

	RETiRet;
}