lka_filter_begin(uint64_t reqid, const char *filter_name)
{
	struct filter_session	*fs;

	if (!filters_inited) {
		tree_init(&sessions);
		filters_inited = 1;
	}

	fs = xcalloc(1, sizeof (struct filter_session));
	fs->id = reqid;
	fs->filter_name = xstrdup(filter_name);
	tree_xset(&sessions, fs->id, fs);

	log_trace(TRACE_FILTERS, "%016"PRIx64" filters session-begin", reqid);
}