static time_t mktime_utc(const struct fake_tm *tm)
{
	time_t result = 0;
	int i;

/* We do allow some ill-formed dates, but we don't do anything special
 * with them and our callers really shouldn't pass them to us.  Do
 * explicitly disallow the ones that would cause invalid array accesses
 * or other algorithm problems. 
 */
	if (tm->tm_mon < 0 || tm->tm_mon > 11 || tm->tm_year < 1970)
		return (time_t) - 1;

/* Convert to a time_t. 
 */
	for (i = 1970; i < tm->tm_year; i++)
		result += 365 + ISLEAP(i);
	for (i = 0; i < tm->tm_mon; i++)
		result += MONTHDAYS[i];
	if (tm->tm_mon > 1 && ISLEAP(tm->tm_year))
		result++;
	result = 24 * (result + tm->tm_mday - 1) + tm->tm_hour;
	result = 60 * result + tm->tm_min;
	result = 60 * result + tm->tm_sec;
	return result;
}