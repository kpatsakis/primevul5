qqueueStart(qqueue_t *pThis) /* this is the ConstructionFinalizer */
{
	DEFiRet;
	uchar pszBuf[64];
	size_t lenBuf;

	ASSERT(pThis != NULL);

	/* we need to do a quick check if our water marks are set plausible. If not,
	 * we correct the most important shortcomings. TODO: do that!!!! -- rgerhards, 2008-03-14
	 */

	/* finalize some initializations that could not yet be done because it is
	 * influenced by properties which might have been set after queueConstruct ()
	 */
	if(pThis->pqParent == NULL) {
		pThis->mut = (pthread_mutex_t *) MALLOC (sizeof (pthread_mutex_t));
		pthread_mutex_init(pThis->mut, NULL);
	} else {
		/* child queue, we need to use parent's mutex */
		DBGOPRINT((obj_t*) pThis, "I am a child\n");
		pThis->mut = pThis->pqParent->mut;
	}

	pthread_mutex_init(&pThis->mutThrdMgmt, NULL);
	pthread_cond_init (&pThis->condDAReady, NULL);
	pthread_cond_init (&pThis->notFull, NULL);
	pthread_cond_init (&pThis->notEmpty, NULL);
	pthread_cond_init (&pThis->belowFullDlyWtrMrk, NULL);
	pthread_cond_init (&pThis->belowLightDlyWtrMrk, NULL);

	/* call type-specific constructor */
	CHKiRet(pThis->qConstruct(pThis)); /* this also sets bIsDA */

	DBGOPRINT((obj_t*) pThis, "type %d, enq-only %d, disk assisted %d, maxFileSz %lld, lqsize %d, pqsize %d, child %d, "
				  "full delay %d, light delay %d, deq batch size %d starting\n",
		  pThis->qType, pThis->bEnqOnly, pThis->bIsDA, pThis->iMaxFileSize,
		  getLogicalQueueSize(pThis), getPhysicalQueueSize(pThis),
		  pThis->pqParent == NULL ? 0 : 1, pThis->iFullDlyMrk, pThis->iLightDlyMrk,
		  pThis->iDeqBatchSize);

	if(pThis->qType == QUEUETYPE_DIRECT)
		FINALIZE;	/* with direct queues, we are already finished... */

	/* create worker thread pools for regular and DA operation.
	 */
	lenBuf = snprintf((char*)pszBuf, sizeof(pszBuf), "%s:Reg", obj.GetName((obj_t*) pThis));
	CHKiRet(wtpConstruct		(&pThis->pWtpReg));
	CHKiRet(wtpSetDbgHdr		(pThis->pWtpReg, pszBuf, lenBuf));
	CHKiRet(wtpSetpfRateLimiter	(pThis->pWtpReg, (rsRetVal (*)(void *pUsr)) RateLimiter));
	CHKiRet(wtpSetpfChkStopWrkr	(pThis->pWtpReg, (rsRetVal (*)(void *pUsr, int)) ChkStopWrkrReg));
	CHKiRet(wtpSetpfGetDeqBatchSize	(pThis->pWtpReg, (rsRetVal (*)(void *pUsr, int*)) GetDeqBatchSize));
	CHKiRet(wtpSetpfDoWork		(pThis->pWtpReg, (rsRetVal (*)(void *pUsr, void *pWti)) ConsumerReg));
	CHKiRet(wtpSetpfObjProcessed	(pThis->pWtpReg, (rsRetVal (*)(void *pUsr, wti_t *pWti)) batchProcessed));
	CHKiRet(wtpSetpmutUsr		(pThis->pWtpReg, pThis->mut));
	CHKiRet(wtpSetpcondBusy		(pThis->pWtpReg, &pThis->notEmpty));
	CHKiRet(wtpSetiNumWorkerThreads	(pThis->pWtpReg, pThis->iNumWorkerThreads));
	CHKiRet(wtpSettoWrkShutdown	(pThis->pWtpReg, pThis->toWrkShutdown));
	CHKiRet(wtpSetpUsr		(pThis->pWtpReg, pThis));
	CHKiRet(wtpConstructFinalize	(pThis->pWtpReg));

	/* set up DA system if we have a disk-assisted queue */
	if(pThis->bIsDA)
		InitDA(pThis, LOCK_MUTEX); /* initiate DA mode */

	DBGOPRINT((obj_t*) pThis, "queue finished initialization\n");

	/* if the queue already contains data, we need to start the correct number of worker threads. This can be
	 * the case when a disk queue has been loaded. If we did not start it here, it would never start.
	 */
	qqueueAdviseMaxWorkers(pThis);
	pThis->bQueueStarted = 1;

finalize_it:
	RETiRet;
}