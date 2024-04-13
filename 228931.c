lka_report_smtp_protocol_server(const char *direction, struct timeval *tv, uint64_t reqid, const char *response)
{
	report_smtp_broadcast(reqid, direction, tv, "protocol-server", "%s\n",
	    response);
}