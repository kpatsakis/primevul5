filter_builtins_rcpt_to(struct filter_session *fs, struct filter *filter, uint64_t reqid, const char *param)
{
	return filter_builtins_global(fs, filter, reqid) ||
	    filter_check_rcpt_to_table(filter, K_MAILADDR, param) ||
	    filter_check_rcpt_to_regex(filter, param);
}