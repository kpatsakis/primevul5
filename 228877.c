lka_report_smtp_timeout(const char *direction, struct timeval *tv, uint64_t reqid)
{
	report_smtp_broadcast(reqid, direction, tv, "timeout", "\n");
}