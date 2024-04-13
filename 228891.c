lka_report_smtp_tx_rcpt(const char *direction, struct timeval *tv, uint64_t reqid, uint32_t msgid, const char *address, int ok)
{
	const char *result;

	switch (ok) {
	case 1:
		result = "ok";
		break;
	case 0:
		result = "permfail";
		break;
	default:
		result = "tempfail";
		break;
	}
	report_smtp_broadcast(reqid, direction, tv, "tx-rcpt", "%08x|%s|%s\n",
	    msgid, result, address);
}