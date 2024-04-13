lka_report_smtp_tx_commit(const char *direction, struct timeval *tv, uint64_t reqid, uint32_t msgid, size_t msgsz)
{
	report_smtp_broadcast(reqid, direction, tv, "tx-commit", "%08x|%zd\n",
	    msgid, msgsz);
}