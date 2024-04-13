lka_report_smtp_link_identify(const char *direction, struct timeval *tv,
    uint64_t reqid, const char *method, const char *heloname)
{
	report_smtp_broadcast(reqid, direction, tv, "link-identify", "%s|%s\n",
	    method, heloname);
}