lka_filter_data_end(uint64_t reqid)
{
	struct filter_session	*fs;

	fs = tree_xget(&sessions, reqid);
	if (fs->io) {
		io_free(fs->io);
		fs->io = NULL;
	}
	log_trace(TRACE_FILTERS, "%016"PRIx64" filters data-end", reqid);
}