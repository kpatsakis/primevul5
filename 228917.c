lka_report_smtp_link_disconnect(const char *direction, struct timeval *tv, uint64_t reqid)
{
	report_smtp_broadcast(reqid, direction, tv, "link-disconnect", "\n");
}