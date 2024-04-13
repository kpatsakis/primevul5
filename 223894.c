StartDA(qqueue_t *pThis)
{
	DEFiRet;
	uchar pszDAQName[128];

	ISOBJ_TYPE_assert(pThis, qqueue);

	/* create message queue */
	CHKiRet(qqueueConstruct(&pThis->pqDA, QUEUETYPE_DISK , 1, 0, pThis->pConsumer));

	/* give it a name */
	snprintf((char*) pszDAQName, sizeof(pszDAQName)/sizeof(uchar), "%s[DA]", obj.GetName((obj_t*) pThis));
	obj.SetName((obj_t*) pThis->pqDA, pszDAQName);

	/* as the created queue is the same object class, we take the
	 * liberty to access its properties directly.
	 */
	pThis->pqDA->pqParent = pThis;

	CHKiRet(qqueueSetpUsr(pThis->pqDA, pThis->pUsr));
	CHKiRet(qqueueSetsizeOnDiskMax(pThis->pqDA, pThis->sizeOnDiskMax));
	CHKiRet(qqueueSetiDeqSlowdown(pThis->pqDA, pThis->iDeqSlowdown));
	CHKiRet(qqueueSetMaxFileSize(pThis->pqDA, pThis->iMaxFileSize));
	CHKiRet(qqueueSetFilePrefix(pThis->pqDA, pThis->pszFilePrefix, pThis->lenFilePrefix));
	CHKiRet(qqueueSetiPersistUpdCnt(pThis->pqDA, pThis->iPersistUpdCnt));
	CHKiRet(qqueueSetbSyncQueueFiles(pThis->pqDA, pThis->bSyncQueueFiles));
	CHKiRet(qqueueSettoActShutdown(pThis->pqDA, pThis->toActShutdown));
	CHKiRet(qqueueSettoEnq(pThis->pqDA, pThis->toEnq));
	CHKiRet(qqueueSetiDeqtWinFromHr(pThis->pqDA, pThis->iDeqtWinFromHr));
	CHKiRet(qqueueSetiDeqtWinToHr(pThis->pqDA, pThis->iDeqtWinToHr));
	CHKiRet(qqueueSettoQShutdown(pThis->pqDA, pThis->toQShutdown));
	CHKiRet(qqueueSetiHighWtrMrk(pThis->pqDA, 0));
	CHKiRet(qqueueSetiDiscardMrk(pThis->pqDA, 0));

	iRet = qqueueStart(pThis->pqDA);
	/* file not found is expected, that means it is no previous QIF available */
	if(iRet != RS_RET_OK && iRet != RS_RET_FILE_NOT_FOUND) {
		errno = 0; /* else an errno is shown in errmsg! */
		errmsg.LogError(errno, iRet, "error starting up disk queue, using pure in-memory mode");
		pThis->bIsDA = 0;	/* disable memory mode */
		FINALIZE; /* something is wrong */
	}

	DBGOPRINT((obj_t*) pThis, "DA queue initialized, disk queue 0x%lx\n",
		  qqueueGetID(pThis->pqDA));

finalize_it:
	if(iRet != RS_RET_OK) {
		if(pThis->pqDA != NULL) {
			qqueueDestruct(&pThis->pqDA);
		}
		DBGOPRINT((obj_t*) pThis, "error %d creating disk queue - giving up.\n", iRet);
		pThis->bIsDA = 0;
	}

	RETiRet;
}