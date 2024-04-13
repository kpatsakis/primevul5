qqueueLoadPersStrmInfoFixup(strm_t *pStrm, qqueue_t __attribute__((unused)) *pThis)
{
	DEFiRet;
	ISOBJ_TYPE_assert(pStrm, strm);
	ISOBJ_TYPE_assert(pThis, qqueue);
	CHKiRet(strm.SetDir(pStrm, glbl.GetWorkDir(), strlen((char*)glbl.GetWorkDir())));
finalize_it:
	RETiRet;
}