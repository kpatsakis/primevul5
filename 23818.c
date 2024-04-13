void cgit_print_date(time_t secs, const char *format, int local_time)
{
	html_txt(fmt_date(secs, format, local_time));
}