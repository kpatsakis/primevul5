static time_t utcTime2gtime(const char *ttime)
{
	char xx[3];
	int year;

	if (strlen(ttime) < 10) {
		gnutls_assert();
		return (time_t) - 1;
	}
	xx[2] = 0;
/* get the year
 */
	memcpy(xx, ttime, 2);	/* year */
	year = atoi(xx);
	ttime += 2;

	if (year > 49)
		year += 1900;
	else
		year += 2000;

	return time2gtime(ttime, year);
}