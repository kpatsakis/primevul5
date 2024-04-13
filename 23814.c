static void print_rel_date(time_t t, double value,
	const char *class, const char *suffix)
{
	htmlf("<span class='%s' title='", class);
	html_attr(fmt_date(t, FMT_LONGDATE, ctx.cfg.local_time));
	htmlf("'>%.0f %s</span>", value, suffix);
}