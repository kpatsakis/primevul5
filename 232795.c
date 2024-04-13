time_t _gnutls_x509_generalTime2gtime(const char *ttime)
{
	char xx[5];
	int year;

	if (strlen(ttime) < 12) {
		gnutls_assert();
		return (time_t) - 1;
	}

	if (strchr(ttime, 'Z') == 0) {
		gnutls_assert();
		/* sorry we don't support it yet
		 */
		return (time_t) - 1;
	}
	xx[4] = 0;

/* get the year
 */
	memcpy(xx, ttime, 4);	/* year */
	year = atoi(xx);
	ttime += 4;

	return time2gtime(ttime, year);
}