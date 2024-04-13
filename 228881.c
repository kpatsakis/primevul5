lka_report_smtp_link_connect(const char *direction, struct timeval *tv, uint64_t reqid, const char *rdns,
    int fcrdns,
    const struct sockaddr_storage *ss_src,
    const struct sockaddr_storage *ss_dest)
{
	struct filter_session *fs;
	char	src[NI_MAXHOST + 5];
	char	dest[NI_MAXHOST + 5];
	uint16_t	src_port = 0;
	uint16_t	dest_port = 0;
	const char     *fcrdns_str;

	if (ss_src->ss_family == AF_INET)
		src_port = ntohs(((const struct sockaddr_in *)ss_src)->sin_port);
	else if (ss_src->ss_family == AF_INET6)
		src_port = ntohs(((const struct sockaddr_in6 *)ss_src)->sin6_port);

	if (ss_dest->ss_family == AF_INET)
		dest_port = ntohs(((const struct sockaddr_in *)ss_dest)->sin_port);
	else if (ss_dest->ss_family == AF_INET6)
		dest_port = ntohs(((const struct sockaddr_in6 *)ss_dest)->sin6_port);

	if (strcmp(ss_to_text(ss_src), "local") == 0) {
		(void)snprintf(src, sizeof src, "unix:%s", SMTPD_SOCKET);
		(void)snprintf(dest, sizeof dest, "unix:%s", SMTPD_SOCKET);
	} else {
		(void)snprintf(src, sizeof src, "%s:%d", ss_to_text(ss_src), src_port);
		(void)snprintf(dest, sizeof dest, "%s:%d", ss_to_text(ss_dest), dest_port);
	}

	switch (fcrdns) {
	case 1:
		fcrdns_str = "pass";
		break;
	case 0:
		fcrdns_str = "fail";
		break;
	default:
		fcrdns_str = "error";
		break;
	}

	fs = tree_xget(&sessions, reqid);
	fs->rdns = xstrdup(rdns);
	fs->fcrdns = fcrdns;
	fs->ss_src = *ss_src;
	fs->ss_dest = *ss_dest;

	report_smtp_broadcast(reqid, direction, tv, "link-connect",
	    "%s|%s|%s|%s\n", rdns, fcrdns_str, src, dest);
}