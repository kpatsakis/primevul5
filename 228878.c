filter_data_query(struct filter *filter, uint64_t token, uint64_t reqid, const char *line)
{
	int	n;
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	n = io_printf(lka_proc_get_io(filter->proc),
	    "filter|%s|%lld.%06ld|smtp-in|data-line|"
	    "%016"PRIx64"|%016"PRIx64"|%s\n",
	    PROTOCOL_VERSION,
	    tv.tv_sec, tv.tv_usec,
	    reqid, token, line);
	if (n == -1)
		fatalx("failed to write to processor");
}