qqueueSetFilePrefix(qqueue_t *pThis, uchar *pszPrefix, size_t iLenPrefix)
{
	DEFiRet;

	free(pThis->pszFilePrefix);
	pThis->pszFilePrefix = NULL;

	if(pszPrefix == NULL) /* just unset the prefix! */
		ABORT_FINALIZE(RS_RET_OK);

	if((pThis->pszFilePrefix = MALLOC(sizeof(uchar) * iLenPrefix + 1)) == NULL)
		ABORT_FINALIZE(RS_RET_OUT_OF_MEMORY);
	memcpy(pThis->pszFilePrefix, pszPrefix, iLenPrefix + 1);
	pThis->lenFilePrefix = iLenPrefix;

finalize_it:
	RETiRet;
}