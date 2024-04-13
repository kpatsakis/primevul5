DataRcvd(ptcpsess_t *pThis, char *pData, size_t iLen)
{
	struct syslogTime stTime;
	DEFiRet;
	pThis->pLstn->rcvdBytes += iLen;
	if(pThis->compressionMode >= COMPRESS_STREAM_ALWAYS)
		iRet =  DataRcvdCompressed(pThis, pData, iLen);
	else
		iRet =  DataRcvdUncompressed(pThis, pData, iLen, &stTime, 0);
	RETiRet;
}