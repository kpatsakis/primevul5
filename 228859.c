filter_builtins_global(struct filter_session *fs, struct filter *filter, uint64_t reqid)
{
	return filter_check_fcrdns(filter, fs->fcrdns) ||
	    filter_check_rdns(filter, fs->rdns) ||
	    filter_check_rdns_table(filter, K_DOMAIN, fs->rdns) ||
	    filter_check_rdns_regex(filter, fs->rdns) ||
	    filter_check_src_table(filter, K_NETADDR, ss_to_text(&fs->ss_src)) ||
	    filter_check_src_regex(filter, ss_to_text(&fs->ss_src)) ||
	    filter_check_helo_table(filter, K_DOMAIN, fs->helo) ||
	    filter_check_helo_regex(filter, fs->helo) ||
	    filter_check_auth(filter, fs->username) ||
	    filter_check_auth_table(filter, K_STRING, fs->username) ||
	    filter_check_auth_table(filter, K_CREDENTIALS, fs->username) ||
	    filter_check_auth_regex(filter, fs->username) ||
	    filter_check_mail_from_table(filter, K_MAILADDR, fs->mail_from) ||
	    filter_check_mail_from_regex(filter, fs->mail_from);
}