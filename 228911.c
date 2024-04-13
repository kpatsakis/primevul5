report_smtp_broadcast(uint64_t reqid, const char *direction, struct timeval *tv, const char *event,
    const char *format, ...)
{
	va_list		ap;
	struct dict	*d;
	struct reporters	*tailq;
	struct reporter_proc	*rp;

	if (strcmp("smtp-in", direction) == 0)
		d = &report_smtp_in;

	else if (strcmp("smtp-out", direction) == 0)
		d = &report_smtp_out;

	else
		fatalx("unexpected direction: %s", direction);

	tailq = dict_xget(d, event);
	TAILQ_FOREACH(rp, tailq, entries) {
		if (!lka_filter_proc_in_session(reqid, rp->name))
			continue;

		va_start(ap, format);
		if (io_printf(lka_proc_get_io(rp->name),
		    "report|%s|%lld.%06ld|%s|%s|%016"PRIx64"%s",
		    PROTOCOL_VERSION, tv->tv_sec, tv->tv_usec, direction,
		    event, reqid, format[0] != '\n' ? "|" : "") == -1 ||
		    io_vprintf(lka_proc_get_io(rp->name), format, ap) == -1)
			fatalx("failed to write to processor");
		va_end(ap);
	}
}