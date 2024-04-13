static rsRetVal qConstructDisk(qqueue_t *pThis)
{
	DEFiRet;
	int bRestarted = 0;

	ASSERT(pThis != NULL);

	/* and now check if there is some persistent information that needs to be read in */
	iRet = qqueueTryLoadPersistedInfo(pThis);
	if(iRet == RS_RET_OK)
		bRestarted = 1;
	else if(iRet != RS_RET_FILE_NOT_FOUND)
			FINALIZE;

	if(bRestarted == 1) {
		;
	} else {
		CHKiRet(strm.Construct(&pThis->tVars.disk.pWrite));
		CHKiRet(strm.SetbSync(pThis->tVars.disk.pWrite, pThis->bSyncQueueFiles));
		CHKiRet(strm.SetDir(pThis->tVars.disk.pWrite, glbl.GetWorkDir(), strlen((char*)glbl.GetWorkDir())));
		CHKiRet(strm.SetiMaxFiles(pThis->tVars.disk.pWrite, 10000000));
		CHKiRet(strm.SettOperationsMode(pThis->tVars.disk.pWrite, STREAMMODE_WRITE));
		CHKiRet(strm.SetsType(pThis->tVars.disk.pWrite, STREAMTYPE_FILE_CIRCULAR));
		CHKiRet(strm.ConstructFinalize(pThis->tVars.disk.pWrite));

		CHKiRet(strm.Construct(&pThis->tVars.disk.pReadDeq));
		CHKiRet(strm.SetbDeleteOnClose(pThis->tVars.disk.pReadDeq, 0));
		CHKiRet(strm.SetDir(pThis->tVars.disk.pReadDeq, glbl.GetWorkDir(), strlen((char*)glbl.GetWorkDir())));
		CHKiRet(strm.SetiMaxFiles(pThis->tVars.disk.pReadDeq, 10000000));
		CHKiRet(strm.SettOperationsMode(pThis->tVars.disk.pReadDeq, STREAMMODE_READ));
		CHKiRet(strm.SetsType(pThis->tVars.disk.pReadDeq, STREAMTYPE_FILE_CIRCULAR));
		CHKiRet(strm.ConstructFinalize(pThis->tVars.disk.pReadDeq));

		CHKiRet(strm.Construct(&pThis->tVars.disk.pReadDel));
		CHKiRet(strm.SetbSync(pThis->tVars.disk.pReadDel, pThis->bSyncQueueFiles));
		CHKiRet(strm.SetbDeleteOnClose(pThis->tVars.disk.pReadDel, 1));
		CHKiRet(strm.SetDir(pThis->tVars.disk.pReadDel, glbl.GetWorkDir(), strlen((char*)glbl.GetWorkDir())));
		CHKiRet(strm.SetiMaxFiles(pThis->tVars.disk.pReadDel, 10000000));
		CHKiRet(strm.SettOperationsMode(pThis->tVars.disk.pReadDel, STREAMMODE_READ));
		CHKiRet(strm.SetsType(pThis->tVars.disk.pReadDel, STREAMTYPE_FILE_CIRCULAR));
		CHKiRet(strm.ConstructFinalize(pThis->tVars.disk.pReadDel));

		CHKiRet(strm.SetFName(pThis->tVars.disk.pWrite,   pThis->pszFilePrefix, pThis->lenFilePrefix));
		CHKiRet(strm.SetFName(pThis->tVars.disk.pReadDeq, pThis->pszFilePrefix, pThis->lenFilePrefix));
		CHKiRet(strm.SetFName(pThis->tVars.disk.pReadDel, pThis->pszFilePrefix, pThis->lenFilePrefix));
	}

	/* now we set (and overwrite in case of a persisted restart) some parameters which
	 * should always reflect the current configuration variables. Be careful by doing so,
	 * for example file name generation must not be changed as that would break the
	 * ability to read existing queue files. -- rgerhards, 2008-01-12
	 */
	CHKiRet(strm.SetiMaxFileSize(pThis->tVars.disk.pWrite, pThis->iMaxFileSize));
	CHKiRet(strm.SetiMaxFileSize(pThis->tVars.disk.pReadDeq, pThis->iMaxFileSize));
	CHKiRet(strm.SetiMaxFileSize(pThis->tVars.disk.pReadDel, pThis->iMaxFileSize));

finalize_it:
	RETiRet;
}