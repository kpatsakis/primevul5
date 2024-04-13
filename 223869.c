static rsRetVal qqueuePersist(qqueue_t *pThis, int bIsCheckpoint)
{
	DEFiRet;
	strm_t *psQIF = NULL; /* Queue Info File */
	uchar pszQIFNam[MAXFNAME];
	size_t lenQIFNam;

	ASSERT(pThis != NULL);

	if(pThis->qType != QUEUETYPE_DISK) {
		if(getPhysicalQueueSize(pThis) > 0) {
			/* This error code is OK, but we will probably not implement this any time
 			 * The reason is that persistence happens via DA queues. But I would like to
			 * leave the code as is, as we so have a hook in case we need one.
			 * -- rgerhards, 2008-01-28
			 */
			ABORT_FINALIZE(RS_RET_NOT_IMPLEMENTED);
		} else
			FINALIZE; /* if the queue is empty, we are happy and done... */
	}

	DBGOPRINT((obj_t*) pThis, "persisting queue to disk, %d entries...\n", getPhysicalQueueSize(pThis));

	/* Construct file name */
	lenQIFNam = snprintf((char*)pszQIFNam, sizeof(pszQIFNam) / sizeof(uchar), "%s/%s.qi",
			     (char*) glbl.GetWorkDir(), (char*)pThis->pszFilePrefix);

	if((bIsCheckpoint != QUEUE_CHECKPOINT) && (getPhysicalQueueSize(pThis) == 0)) {
		if(pThis->bNeedDelQIF) {
			unlink((char*)pszQIFNam);
			pThis->bNeedDelQIF = 0;
		}
		/* indicate spool file needs to be deleted */
		if(pThis->tVars.disk.pReadDel != NULL) /* may be NULL if we had a startup failure! */
			CHKiRet(strm.SetbDeleteOnClose(pThis->tVars.disk.pReadDel, 1));
		FINALIZE; /* nothing left to do, so be happy */
	}

	CHKiRet(strm.Construct(&psQIF));
	CHKiRet(strm.SettOperationsMode(psQIF, STREAMMODE_WRITE_TRUNC));
	CHKiRet(strm.SetbSync(psQIF, pThis->bSyncQueueFiles));
	CHKiRet(strm.SetsType(psQIF, STREAMTYPE_FILE_SINGLE));
	CHKiRet(strm.SetFName(psQIF, pszQIFNam, lenQIFNam));
	CHKiRet(strm.ConstructFinalize(psQIF));

	/* first, write the property bag for ourselfs
	 * And, surprisingly enough, we currently need to persist only the size of the
	 * queue. All the rest is re-created with then-current config parameters when the
	 * queue is re-created. Well, we'll also save the current queue type, just so that
	 * we know when somebody has changed the queue type... -- rgerhards, 2008-01-11
	 */
	CHKiRet(obj.BeginSerializePropBag(psQIF, (obj_t*) pThis));
	objSerializeSCALAR(psQIF, iQueueSize, INT);
	objSerializeSCALAR(psQIF, tVars.disk.sizeOnDisk, INT64);
	objSerializeSCALAR(psQIF, tVars.disk.bytesRead, INT64);
	CHKiRet(obj.EndSerialize(psQIF));

	/* now persist the stream info */
	CHKiRet(strm.Serialize(pThis->tVars.disk.pWrite, psQIF));
	CHKiRet(strm.Serialize(pThis->tVars.disk.pReadDel, psQIF));
	
	/* tell the input file object that it must not delete the file on close if the queue
	 * is non-empty - but only if we are not during a simple checkpoint
	 */
	if(bIsCheckpoint != QUEUE_CHECKPOINT) {
		CHKiRet(strm.SetbDeleteOnClose(pThis->tVars.disk.pReadDel, 0));
	}

	/* we have persisted the queue object. So whenever it comes to an empty queue,
	 * we need to delete the QIF. Thus, we indicte that need.
	 */
	pThis->bNeedDelQIF = 1;

finalize_it:
	if(psQIF != NULL)
		strm.Destruct(&psQIF);

	RETiRet;
}