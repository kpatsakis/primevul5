lka_report_smtp_tx_rollback(const char *direction, struct timeval *tv, uint64_t reqid, uint32_t msgid)
{
	report_smtp_broadcast(reqid, direction, tv, "tx-rollback", "%08x\n",
	    msgid);
}