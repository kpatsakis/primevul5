lka_report_smtp_link_greeting(const char *direction, uint64_t reqid,
    struct timeval *tv, const char *domain)
{
	report_smtp_broadcast(reqid, direction, tv, "link-greeting", "%s\n",
	    domain);
}