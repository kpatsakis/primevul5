static int qqueueChkDiscardMsg(qqueue_t *pThis, int iQueueSize, void *pUsr)
{
	DEFiRet;
	rsRetVal iRetLocal;
	int iSeverity;

	ISOBJ_TYPE_assert(pThis, qqueue);
	ISOBJ_assert(pUsr);

	if(pThis->iDiscardMrk > 0 && iQueueSize >= pThis->iDiscardMrk) {
		iRetLocal = objGetSeverity(pUsr, &iSeverity);
		if(iRetLocal == RS_RET_OK && iSeverity >= pThis->iDiscardSeverity) {
			DBGOPRINT((obj_t*) pThis, "queue nearly full (%d entries), discarded severity %d message\n",
				  iQueueSize, iSeverity);
			objDestruct(pUsr);
			ABORT_FINALIZE(RS_RET_QUEUE_FULL);
		} else {
			DBGOPRINT((obj_t*) pThis, "queue nearly full (%d entries), but could not drop msg "
				  "(iRet: %d, severity %d)\n", iQueueSize, iRetLocal, iSeverity);
		}
	}

finalize_it:
	RETiRet;
}