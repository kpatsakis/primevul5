filter_result_disconnect(uint64_t reqid, const char *message)
{
	m_create(p_pony, IMSG_FILTER_SMTP_PROTOCOL, 0, 0, -1);
	m_add_id(p_pony, reqid);
	m_add_int(p_pony, FILTER_DISCONNECT);
	m_add_string(p_pony, message);
	m_close(p_pony);
}