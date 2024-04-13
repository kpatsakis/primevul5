lka_report_smtp_tx_envelope(const char *direction, struct timeval *tv, uint64_t reqid, uint32_t msgid, uint64_t evpid)
{
	report_smtp_broadcast(reqid, direction, tv, "tx-envelope",
	    "%08x|%016"PRIx64"\n", msgid, evpid);
}