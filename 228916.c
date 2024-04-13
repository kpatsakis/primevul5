filter_protocol_query(struct filter *filter, uint64_t token, uint64_t reqid, const char *phase, const char *param)
{
	int	n;
	struct filter_session	*fs;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	
	fs = tree_xget(&sessions, reqid);
	if (strcmp(phase, "connect") == 0)
		n = io_printf(lka_proc_get_io(filter->proc),
		    "filter|%s|%lld.%06ld|smtp-in|%s|%016"PRIx64"|%016"PRIx64"|%s|%s\n",
		    PROTOCOL_VERSION,
		    tv.tv_sec, tv.tv_usec,
		    phase, reqid, token, fs->rdns, param);
	else
		n = io_printf(lka_proc_get_io(filter->proc),
		    "filter|%s|%lld.%06ld|smtp-in|%s|%016"PRIx64"|%016"PRIx64"|%s\n",
		    PROTOCOL_VERSION,
		    tv.tv_sec, tv.tv_usec,
		    phase, reqid, token, param);
	if (n == -1)
		fatalx("failed to write to processor");
}