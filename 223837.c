InitDA(qqueue_t *pThis, int bLockMutex)
{
	DEFiRet;
	DEFVARS_mutexProtection;
	uchar pszBuf[64];
	size_t lenBuf;

	BEGIN_MTX_PROTECTED_OPERATIONS(pThis->mut, bLockMutex);
	/* check if we already have a DA worker pool. If not, initiate one. Please note that the
	 * pool is created on first need but never again destructed (until the queue is). This
	 * is intentional. We assume that when we need it once, we may also need it on another
	 * occasion. Ressources used are quite minimal when no worker is running.
	 * rgerhards, 2008-01-24
	 * NOTE: this is the DA worker *pool*, not the DA queue!
	 */
	lenBuf = snprintf((char*)pszBuf, sizeof(pszBuf), "%s:DAwpool", obj.GetName((obj_t*) pThis));
	CHKiRet(wtpConstruct		(&pThis->pWtpDA));
	CHKiRet(wtpSetDbgHdr		(pThis->pWtpDA, pszBuf, lenBuf));
	CHKiRet(wtpSetpfChkStopWrkr	(pThis->pWtpDA, (rsRetVal (*)(void *pUsr, int)) qqueueChkStopWrkrDA));
	CHKiRet(wtpSetpfGetDeqBatchSize	(pThis->pWtpDA, (rsRetVal (*)(void *pUsr, int*)) GetDeqBatchSize));
	CHKiRet(wtpSetpfDoWork		(pThis->pWtpDA, (rsRetVal (*)(void *pUsr, void *pWti)) ConsumerDA));
	CHKiRet(wtpSetpfObjProcessed	(pThis->pWtpDA, (rsRetVal (*)(void *pUsr, wti_t *pWti)) batchProcessed));
	CHKiRet(wtpSetpmutUsr		(pThis->pWtpDA, pThis->mut));
	CHKiRet(wtpSetpcondBusy		(pThis->pWtpDA, &pThis->notEmpty));
	CHKiRet(wtpSetiNumWorkerThreads	(pThis->pWtpDA, 1));
	CHKiRet(wtpSettoWrkShutdown	(pThis->pWtpDA, pThis->toWrkShutdown));
	CHKiRet(wtpSetpUsr		(pThis->pWtpDA, pThis));
	CHKiRet(wtpConstructFinalize	(pThis->pWtpDA));
	/* if we reach this point, we have a "good" DA worker pool */

	/* now construct the actual queue (if it does not already exist) */
	if(pThis->pqDA == NULL) {
		CHKiRet(StartDA(pThis));
	}

finalize_it:
	END_MTX_PROTECTED_OPERATIONS(pThis->mut);
	RETiRet;
}