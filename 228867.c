lka_report_smtp_link_auth(const char *direction, struct timeval *tv, uint64_t reqid,
    const char *username, const char *result)
{
	struct filter_session *fs;

	if (strcmp(result, "pass") == 0) {
		fs = tree_xget(&sessions, reqid);
		fs->username = xstrdup(username);
	}
	report_smtp_broadcast(reqid, direction, tv, "link-auth", "%s|%s\n",
	    username, result);
}