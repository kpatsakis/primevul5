rsRetVal qqueueConstruct(qqueue_t **ppThis, queueType_t qType, int iWorkerThreads,
		        int iMaxQueueSize, rsRetVal (*pConsumer)(void*, batch_t*,int*))
{
	DEFiRet;
	qqueue_t *pThis;

	ASSERT(ppThis != NULL);
	ASSERT(pConsumer != NULL);
	ASSERT(iWorkerThreads >= 0);

	CHKmalloc(pThis = (qqueue_t *)calloc(1, sizeof(qqueue_t)));

	/* we have an object, so let's fill the properties */
	objConstructSetObjInfo(pThis);
	if((pThis->pszSpoolDir = (uchar*) strdup((char*)glbl.GetWorkDir())) == NULL)
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);

	/* set some water marks so that we have useful defaults if none are set specifically */
	pThis->iFullDlyMrk  = iMaxQueueSize - (iMaxQueueSize / 100) *  3; /* default 97% */
	pThis->iLightDlyMrk = iMaxQueueSize - (iMaxQueueSize / 100) * 30; /* default 70% */

	pThis->lenSpoolDir = ustrlen(pThis->pszSpoolDir);
	pThis->iMaxFileSize = 1024 * 1024; /* default is 1 MiB */
	pThis->iQueueSize = 0;
	pThis->nLogDeq = 0;
	pThis->iMaxQueueSize = iMaxQueueSize;
	pThis->pConsumer = pConsumer;
	pThis->iNumWorkerThreads = iWorkerThreads;
	pThis->iDeqtWinToHr = 25; /* disable time-windowed dequeuing by default */
	pThis->iDeqBatchSize = 8; /* conservative default, should still provide good performance */

	pThis->pszFilePrefix = NULL;
	pThis->qType = qType;

	/* set type-specific handlers and other very type-specific things (we can not totally hide it...) */
	switch(qType) {
		case QUEUETYPE_FIXED_ARRAY:
			pThis->qConstruct = qConstructFixedArray;
			pThis->qDestruct = qDestructFixedArray;
			pThis->qAdd = qAddFixedArray;
			pThis->qDeq = qDeqFixedArray;
			pThis->qDel = qDelFixedArray;
			pThis->MultiEnq = qqueueMultiEnqObjNonDirect;
			break;
		case QUEUETYPE_LINKEDLIST:
			pThis->qConstruct = qConstructLinkedList;
			pThis->qDestruct = qDestructLinkedList;
			pThis->qAdd = qAddLinkedList;
			pThis->qDeq = (rsRetVal (*)(qqueue_t*,void**)) qDeqLinkedList;
			pThis->qDel = (rsRetVal (*)(qqueue_t*)) qDelLinkedList;
			pThis->MultiEnq = qqueueMultiEnqObjNonDirect;
			break;
		case QUEUETYPE_DISK:
			pThis->qConstruct = qConstructDisk;
			pThis->qDestruct = qDestructDisk;
			pThis->qAdd = qAddDisk;
			pThis->qDeq = qDeqDisk;
			pThis->qDel = qDelDisk;
			pThis->MultiEnq = qqueueMultiEnqObjNonDirect;
			/* special handling */
			pThis->iNumWorkerThreads = 1; /* we need exactly one worker */
			break;
		case QUEUETYPE_DIRECT:
			pThis->qConstruct = qConstructDirect;
			pThis->qDestruct = qDestructDirect;
			pThis->qAdd = qAddDirect;
			pThis->qDel = qDelDirect;
			pThis->MultiEnq = qqueueMultiEnqObjDirect;
			break;
	}

	INIT_ATOMIC_HELPER_MUT(pThis->mutQueueSize);
	INIT_ATOMIC_HELPER_MUT(pThis->mutLogDeq);

finalize_it:
	OBJCONSTRUCT_CHECK_SUCCESS_AND_CLEANUP
	RETiRet;
}