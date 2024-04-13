qqueueChkIsDA(qqueue_t *pThis)
{
	DEFiRet;

	ISOBJ_TYPE_assert(pThis, qqueue);
	if(pThis->pszFilePrefix != NULL) {
		pThis->bIsDA = 1;
		DBGOPRINT((obj_t*) pThis, "is disk-assisted, disk will be used on demand\n");
	} else {
		DBGOPRINT((obj_t*) pThis, "is NOT disk-assisted\n");
	}

	RETiRet;
}