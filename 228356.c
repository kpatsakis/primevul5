HRESULT Http::HrGetUserAgent(std::string *strUserAgent)
{
	if (m_strUserAgent.empty())
		return MAPI_E_NOT_FOUND;
	strUserAgent -> assign(m_strUserAgent);
	return hrSuccess;
}