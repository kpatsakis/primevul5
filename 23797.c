static const char *fmt_date(time_t secs, const char *format, int local_time)
{
	static char buf[64];
	struct tm *time;

	if (!secs)
		return "";
	if (local_time)
		time = localtime(&secs);
	else
		time = gmtime(&secs);
	strftime(buf, sizeof(buf)-1, format, time);
	return buf;
}