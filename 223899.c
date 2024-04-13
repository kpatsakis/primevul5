qqueueTryLoadPersistedInfo(qqueue_t *pThis)
{
	DEFiRet;
	strm_t *psQIF = NULL;
	uchar pszQIFNam[MAXFNAME];
	size_t lenQIFNam;
	struct stat stat_buf;

	ISOBJ_TYPE_assert(pThis, qqueue);

	/* Construct file name */
	lenQIFNam = snprintf((char*)pszQIFNam, sizeof(pszQIFNam) / sizeof(uchar), "%s/%s.qi",
			     (char*) glbl.GetWorkDir(), (char*)pThis->pszFilePrefix);

	/* check if the file exists */
	if(stat((char*) pszQIFNam, &stat_buf) == -1) {
		if(errno == ENOENT) {
			DBGOPRINT((obj_t*) pThis, "clean startup, no .qi file found\n");
			ABORT_FINALIZE(RS_RET_FILE_NOT_FOUND);
		} else {
			DBGOPRINT((obj_t*) pThis, "error %d trying to access .qi file\n", errno);
			ABORT_FINALIZE(RS_RET_IO_ERROR);
		}
	}

	/* If we reach this point, we have a .qi file */

	CHKiRet(strm.Construct(&psQIF));
	CHKiRet(strm.SettOperationsMode(psQIF, STREAMMODE_READ));
	CHKiRet(strm.SetsType(psQIF, STREAMTYPE_FILE_SINGLE));
	CHKiRet(strm.SetFName(psQIF, pszQIFNam, lenQIFNam));
	CHKiRet(strm.ConstructFinalize(psQIF));

	/* first, we try to read the property bag for ourselfs */
	CHKiRet(obj.DeserializePropBag((obj_t*) pThis, psQIF));
	
	/* then the stream objects (same order as when persisted!) */
	CHKiRet(obj.Deserialize(&pThis->tVars.disk.pWrite, (uchar*) "strm", psQIF,
			       (rsRetVal(*)(obj_t*,void*))qqueueLoadPersStrmInfoFixup, pThis));
	CHKiRet(obj.Deserialize(&pThis->tVars.disk.pReadDel, (uchar*) "strm", psQIF,
			       (rsRetVal(*)(obj_t*,void*))qqueueLoadPersStrmInfoFixup, pThis));

	/* create a duplicate for the read "pointer".
	 */

	CHKiRet(strm.Dup(pThis->tVars.disk.pReadDel, &pThis->tVars.disk.pReadDeq));
	CHKiRet(strm.SetbDeleteOnClose(pThis->tVars.disk.pReadDeq, 0)); /* deq must NOT delete the files! */
	CHKiRet(strm.ConstructFinalize(pThis->tVars.disk.pReadDeq));

	CHKiRet(strm.SeekCurrOffs(pThis->tVars.disk.pWrite));
	CHKiRet(strm.SeekCurrOffs(pThis->tVars.disk.pReadDel));
	CHKiRet(strm.SeekCurrOffs(pThis->tVars.disk.pReadDeq));

	/* OK, we could successfully read the file, so we now can request that it be
	 * deleted when we are done with the persisted information.
	 */
	pThis->bNeedDelQIF = 1;

finalize_it:
	if(psQIF != NULL)
		strm.Destruct(&psQIF);

	if(iRet != RS_RET_OK) {
		DBGOPRINT((obj_t*) pThis, "error %d reading .qi file - can not read persisted info (if any)\n",
			  iRet);
	}

	RETiRet;
}