lka_report_smtp_link_tls(const char *direction, struct timeval *tv, uint64_t reqid, const char *ciphers)
{
	report_smtp_broadcast(reqid, direction, tv, "link-tls", "%s\n",
	    ciphers);
}