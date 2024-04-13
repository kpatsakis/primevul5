static rsRetVal qDeqDisk(qqueue_t *pThis, void **ppUsr)
{
	DEFiRet;

	CHKiRet(obj.Deserialize(ppUsr, (uchar*) "msg", pThis->tVars.disk.pReadDeq, NULL, NULL));

finalize_it:
	RETiRet;
}