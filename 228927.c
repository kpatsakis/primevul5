lka_report_filter_report(uint64_t reqid, const char *name, int builtin,
    const char *direction, struct timeval *tv, const char *message)
{
	report_smtp_broadcast(reqid, direction, tv, "filter-report",
	    "%s|%s|%s\n", builtin ? "builtin" : "proc",
	    name, message);
}