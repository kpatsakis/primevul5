static rsRetVal qDelDisk(qqueue_t *pThis)
{
	obj_t *pDummyObj;	/* we need to deserialize it... */
	DEFiRet;

	int64 offsIn;
	int64 offsOut;

	CHKiRet(strm.GetCurrOffset(pThis->tVars.disk.pReadDel, &offsIn));
	CHKiRet(obj.Deserialize(&pDummyObj, (uchar*) "msg", pThis->tVars.disk.pReadDel, NULL, NULL));
	objDestruct(pDummyObj);
	CHKiRet(strm.GetCurrOffset(pThis->tVars.disk.pReadDel, &offsOut));

	/* This time it is a bit tricky: we free disk space only upon file deletion. So we need
	 * to keep track of what we have read until we get an out-offset that is lower than the
	 * in-offset (which indicates file change). Then, we can subtract the whole thing from
	 * the on-disk size. -- rgerhards, 2008-01-30
	 */
	if(offsIn < offsOut) {
		pThis->tVars.disk.bytesRead += offsOut - offsIn;
	} else {
		pThis->tVars.disk.sizeOnDisk -= pThis->tVars.disk.bytesRead;
		pThis->tVars.disk.bytesRead = offsOut;
		DBGOPRINT((obj_t*) pThis, "a file has been deleted, now %lld octets disk space used\n", pThis->tVars.disk.sizeOnDisk);
		/* awake possibly waiting enq process */
		pthread_cond_signal(&pThis->notFull); /* we hold the mutex while we are in here! */
	}

finalize_it:
	RETiRet;
}