doEnqSingleObj(qqueue_t *pThis, flowControl_t flowCtlType, void *pUsr)
{
	DEFiRet;
	struct timespec t;

	/* first check if we need to discard this message (which will cause CHKiRet() to exit)
	 */
	CHKiRet(qqueueChkDiscardMsg(pThis, pThis->iQueueSize, pUsr));

	/* handle flow control
	 * There are two different flow control mechanisms: basic and advanced flow control.
	 * Basic flow control has always been implemented and protects the queue structures
	 * in that it makes sure no more data is enqueued than the queue is configured to
	 * support. Enhanced flow control is being added today. There are some sources which
	 * can easily be stopped, e.g. a file reader. This is the case because it is unlikely
	 * that blocking those sources will have negative effects (after all, the file is
	 * continued to be written). Other sources can somewhat be blocked (e.g. the kernel
	 * log reader or the local log stream reader): in general, nothing is lost if messages
	 * from these sources are not picked up immediately. HOWEVER, they can not block for
	 * an extended period of time, as this either causes message loss or - even worse - some
	 * other bad effects (e.g. unresponsive system in respect to the main system log socket).
	 * Finally, there are some (few) sources which can not be blocked at all. UDP syslog is
	 * a prime example. If a UDP message is not received, it is simply lost. So we can't
	 * do anything against UDP sockets that come in too fast. The core idea of advanced
	 * flow control is that we take into account the different natures of the sources and
	 * select flow control mechanisms that fit these needs. This also means, in the end
	 * result, that non-blockable sources like UDP syslog receive priority in the system.
	 * It's a side effect, but a good one ;) -- rgerhards, 2008-03-14
	 */
	if(flowCtlType == eFLOWCTL_FULL_DELAY) {
		while(pThis->iQueueSize >= pThis->iFullDlyMrk) {
			DBGOPRINT((obj_t*) pThis, "enqueueMsg: FullDelay mark reached for full delayable message - blocking.\n");
			pthread_cond_wait(&pThis->belowFullDlyWtrMrk, pThis->mut); /* TODO error check? But what do then? */
		}
	} else if(flowCtlType == eFLOWCTL_LIGHT_DELAY) {
		if(pThis->iQueueSize >= pThis->iLightDlyMrk) {
			DBGOPRINT((obj_t*) pThis, "enqueueMsg: LightDelay mark reached for light delayable message - blocking a bit.\n");
			timeoutComp(&t, 1000); /* 1000 millisconds = 1 second TODO: make configurable */
			pthread_cond_timedwait(&pThis->belowLightDlyWtrMrk, pThis->mut, &t); /* TODO error check? But what do then? */
		}
	}

	/* from our regular flow control settings, we are now ready to enqueue the object.
	 * However, we now need to do a check if the queue permits to add more data. If that
	 * is not the case, basic flow control enters the field, which means we wait for
	 * the queue to become ready or drop the new message. -- rgerhards, 2008-03-14
	 */
	while(   (pThis->iMaxQueueSize > 0 && pThis->iQueueSize >= pThis->iMaxQueueSize)
	      || (pThis->qType == QUEUETYPE_DISK && pThis->sizeOnDiskMax != 0
	      	  && pThis->tVars.disk.sizeOnDisk > pThis->sizeOnDiskMax)) {
		DBGOPRINT((obj_t*) pThis, "enqueueMsg: queue FULL - waiting to drain.\n");
		timeoutComp(&t, pThis->toEnq);
// TODO : handle enqOnly => discard!
		if(pthread_cond_timedwait(&pThis->notFull, pThis->mut, &t) != 0) {
			DBGOPRINT((obj_t*) pThis, "enqueueMsg: cond timeout, dropping message!\n");
			objDestruct(pUsr);
			ABORT_FINALIZE(RS_RET_QUEUE_FULL);
		}
	}

	/* and finally enqueue the message */
	CHKiRet(qqueueAdd(pThis, pUsr));

finalize_it:
	RETiRet;
}