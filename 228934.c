lka_report_smtp_protocol_client(const char *direction, struct timeval *tv, uint64_t reqid, const char *command)
{
	report_smtp_broadcast(reqid, direction, tv, "protocol-client", "%s\n",
	    command);
}