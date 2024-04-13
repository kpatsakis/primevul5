DataRcvdCompressed(ptcpsess_t *pThis, char *buf, size_t len)
{
	struct syslogTime stTime;
	time_t ttGenTime;
	int zRet;	/* zlib return state */
	unsigned outavail;
	uchar zipBuf[64*1024]; // TODO: alloc on heap, and much larger (512KiB? batch size!)
	DEFiRet;
	// TODO: can we do stats counters? Even if they are not 100% correct under all cases,
	// by simply updating the input and output sizes?
	uint64_t outtotal;

	datetime.getCurrTime(&stTime, &ttGenTime, TIME_IN_LOCALTIME);
	outtotal = 0;

	if(!pThis->bzInitDone) {
		/* allocate deflate state */
		pThis->zstrm.zalloc = Z_NULL;
		pThis->zstrm.zfree = Z_NULL;
		pThis->zstrm.opaque = Z_NULL;
		zRet = inflateInit(&pThis->zstrm);
		if(zRet != Z_OK) {
			DBGPRINTF("imptcp: error %d returned from zlib/inflateInit()\n", zRet);
			ABORT_FINALIZE(RS_RET_ZLIB_ERR);
		}
		pThis->bzInitDone = RSTRUE;
	}

	pThis->zstrm.next_in = (Bytef*) buf;
	pThis->zstrm.avail_in = len;
	/* run inflate() on buffer until everything has been uncompressed */
	do {
		DBGPRINTF("imptcp: in inflate() loop, avail_in %d, total_in %ld\n", pThis->zstrm.avail_in, pThis->zstrm.total_in);
		pThis->zstrm.avail_out = sizeof(zipBuf);
		pThis->zstrm.next_out = zipBuf;
		zRet = inflate(&pThis->zstrm, Z_SYNC_FLUSH);    /* no bad return value */
		//zRet = inflate(&pThis->zstrm, Z_NO_FLUSH);    /* no bad return value */
		DBGPRINTF("after inflate, ret %d, avail_out %d\n", zRet, pThis->zstrm.avail_out);
		outavail = sizeof(zipBuf) - pThis->zstrm.avail_out;
		if(outavail != 0) {
			outtotal += outavail;
			pThis->pLstn->rcvdDecompressed += outavail;
			CHKiRet(DataRcvdUncompressed(pThis, (char*)zipBuf, outavail, &stTime, ttGenTime));
		}
	} while (pThis->zstrm.avail_out == 0);

	dbgprintf("end of DataRcvCompress, sizes: in %lld, out %llu\n", (long long) len, (long long unsigned) outtotal);
finalize_it:
	RETiRet;
}