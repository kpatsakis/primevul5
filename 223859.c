static rsRetVal qAddDisk(qqueue_t *pThis, void* pUsr)
{
	DEFiRet;
	number_t nWriteCount;

	ASSERT(pThis != NULL);

	CHKiRet(strm.SetWCntr(pThis->tVars.disk.pWrite, &nWriteCount));
	CHKiRet((objSerialize(pUsr))(pUsr, pThis->tVars.disk.pWrite));
	CHKiRet(strm.Flush(pThis->tVars.disk.pWrite));
	CHKiRet(strm.SetWCntr(pThis->tVars.disk.pWrite, NULL)); /* no more counting for now... */

	pThis->tVars.disk.sizeOnDisk += nWriteCount;

	/* we have enqueued the user element to disk. So we now need to destruct
	 * the in-memory representation. The instance will be re-created upon
	 * dequeue. -- rgerhards, 2008-07-09
	 */
	objDestruct(pUsr);

	DBGOPRINT((obj_t*) pThis, "write wrote %lld octets to disk, queue disk size now %lld octets, EnqOnly:%d\n",
		   nWriteCount, pThis->tVars.disk.sizeOnDisk, pThis->bEnqOnly);

finalize_it:
	RETiRet;
}