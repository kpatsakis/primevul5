void cgit_print_age(time_t t, time_t max_relative, const char *format)
{
	time_t now, secs;

	if (!t)
		return;
	time(&now);
	secs = now - t;
	if (secs < 0)
		secs = 0;

	if (secs > max_relative && max_relative >= 0) {
		html("<span title='");
		html_attr(fmt_date(t, FMT_LONGDATE, ctx.cfg.local_time));
		html("'>");
		cgit_print_date(t, format, ctx.cfg.local_time);
		html("</span>");
		return;
	}

	if (secs < TM_HOUR * 2) {
		print_rel_date(t, secs * 1.0 / TM_MIN, "age-mins", "min.");
		return;
	}
	if (secs < TM_DAY * 2) {
		print_rel_date(t, secs * 1.0 / TM_HOUR, "age-hours", "hours");
		return;
	}
	if (secs < TM_WEEK * 2) {
		print_rel_date(t, secs * 1.0 / TM_DAY, "age-days", "days");
		return;
	}
	if (secs < TM_MONTH * 2) {
		print_rel_date(t, secs * 1.0 / TM_WEEK, "age-weeks", "weeks");
		return;
	}
	if (secs < TM_YEAR * 2) {
		print_rel_date(t, secs * 1.0 / TM_MONTH, "age-months", "months");
		return;
	}
	print_rel_date(t, secs * 1.0 / TM_YEAR, "age-years", "years");
}