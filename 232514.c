void t2p_pdf_currenttime(T2P* t2p)
{
	struct tm* currenttime;
	time_t timenow;

	if (time(&timenow) == (time_t) -1) {
		TIFFError(TIFF2PDF_MODULE,
			  "Can't get the current time: %s", strerror(errno));
		timenow = (time_t) 0;
	}

	currenttime = localtime(&timenow);
	snprintf(t2p->pdf_datetime, sizeof(t2p->pdf_datetime),
		 "D:%.4u%.2u%.2u%.2u%.2u%.2u",
		 TIFFmin((unsigned) currenttime->tm_year + 1900U,9999U),
		 TIFFmin((unsigned) currenttime->tm_mon + 1U,12U),   /* 0-11 + 1 */
		 TIFFmin((unsigned) currenttime->tm_mday,31U),       /* 1-31 */
		 TIFFmin((unsigned) currenttime->tm_hour,23U),       /* 0-23 */
		 TIFFmin((unsigned) currenttime->tm_min,59U),        /* 0-59 */
		 TIFFmin((unsigned) (currenttime->tm_sec),60U));     /* 0-60 */

	return;
}