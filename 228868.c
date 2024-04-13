lka_filter_end(uint64_t reqid)
{
	struct filter_session	*fs;

	fs = tree_xpop(&sessions, reqid);
	free(fs->rdns);
	free(fs->helo);
	free(fs->mail_from);
	free(fs->username);
	free(fs->lastparam);
	free(fs->filter_name);
	free(fs);
	log_trace(TRACE_FILTERS, "%016"PRIx64" filters session-end", reqid);
}