filter_result_junk(uint64_t reqid)
{
	m_create(p_pony, IMSG_FILTER_SMTP_PROTOCOL, 0, 0, -1);
	m_add_id(p_pony, reqid);
	m_add_int(p_pony, FILTER_JUNK);
	m_close(p_pony);
}