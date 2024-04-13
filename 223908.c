static rsRetVal qDestructDisk(qqueue_t *pThis)
{
	DEFiRet;
	
	ASSERT(pThis != NULL);
	
	if(pThis->tVars.disk.pWrite != NULL)
		strm.Destruct(&pThis->tVars.disk.pWrite);
	if(pThis->tVars.disk.pReadDeq != NULL)
		strm.Destruct(&pThis->tVars.disk.pReadDeq);
	if(pThis->tVars.disk.pReadDel != NULL)
		strm.Destruct(&pThis->tVars.disk.pReadDel);

	RETiRet;
}