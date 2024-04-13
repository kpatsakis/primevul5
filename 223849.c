qqueueEnqObjDirect(qqueue_t *pThis, void *pUsr)
{
	DEFiRet;
	ISOBJ_TYPE_assert(pThis, qqueue);
	iRet = qAddDirect(pThis, pUsr);
	RETiRet;
}