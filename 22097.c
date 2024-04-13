doZipFinish(ptcpsess_t *pSess)
{
	int zRet;	/* zlib return state */
	DEFiRet;
	unsigned outavail;
	struct syslogTime stTime;
	uchar zipBuf[32*1024]; // TODO: use "global" one from pSess

	if(!pSess->bzInitDone)
		goto done;

	pSess->zstrm.avail_in = 0;
	/* run inflate() on buffer until everything has been compressed */
	do {
		DBGPRINTF("doZipFinish: in inflate() loop, avail_in %d, total_in %ld\n", pSess->zstrm.avail_in,
		pSess->zstrm.total_in);
		pSess->zstrm.avail_out = sizeof(zipBuf);
		pSess->zstrm.next_out = zipBuf;
		zRet = inflate(&pSess->zstrm, Z_FINISH);    /* no bad return value */
		DBGPRINTF("after inflate, ret %d, avail_out %d\n", zRet, pSess->zstrm.avail_out);
		outavail = sizeof(zipBuf) - pSess->zstrm.avail_out;
		if(outavail != 0) {
			pSess->pLstn->rcvdDecompressed += outavail;
			CHKiRet(DataRcvdUncompressed(pSess, (char*)zipBuf, outavail, &stTime, 0)); // TODO: query time!
		}
	} while (pSess->zstrm.avail_out == 0);

finalize_it:
	zRet = inflateEnd(&pSess->zstrm);
	if(zRet != Z_OK) {
		DBGPRINTF("imptcp: error %d returned from zlib/inflateEnd()\n", zRet);
	}

	pSess->bzInitDone = 0;
done:	RETiRet;
}