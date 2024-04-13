filter_result_rewrite(uint64_t reqid, const char *param)
{
	m_create(p_pony, IMSG_FILTER_SMTP_PROTOCOL, 0, 0, -1);
	m_add_id(p_pony, reqid);
	m_add_int(p_pony, FILTER_REWRITE);
	m_add_string(p_pony, param);
	m_close(p_pony);
}