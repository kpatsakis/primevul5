static time_t time2gtime(const char *ttime, int year)
{
	char xx[4];
	struct fake_tm etime;

	if (strlen(ttime) < 8) {
		gnutls_assert();
		return (time_t) - 1;
	}

	etime.tm_year = year;

	/* In order to work with 32 bit
	 * time_t.
	 */
	if (sizeof(time_t) <= 4 && etime.tm_year >= 2038)
		return (time_t) 2145914603;	/* 2037-12-31 23:23:23 */

	if (etime.tm_year < 1970)
		return (time_t) 0;

	xx[2] = 0;

/* get the month
 */
	memcpy(xx, ttime, 2);	/* month */
	etime.tm_mon = atoi(xx) - 1;
	ttime += 2;

/* get the day
 */
	memcpy(xx, ttime, 2);	/* day */
	etime.tm_mday = atoi(xx);
	ttime += 2;

/* get the hour
 */
	memcpy(xx, ttime, 2);	/* hour */
	etime.tm_hour = atoi(xx);
	ttime += 2;

/* get the minutes
 */
	memcpy(xx, ttime, 2);	/* minutes */
	etime.tm_min = atoi(xx);
	ttime += 2;

	if (strlen(ttime) >= 2) {
		memcpy(xx, ttime, 2);
		etime.tm_sec = atoi(xx);
	} else
		etime.tm_sec = 0;

	return mktime_utc(&etime);
}