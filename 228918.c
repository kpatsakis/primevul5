lka_report_smtp_tx_data(const char *direction, struct timeval *tv, uint64_t reqid, uint32_t msgid, int ok)
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
	report_smtp_broadcast(reqid, direction, tv, "tx-data", "%08x|%s\n",
	    msgid, result);
}