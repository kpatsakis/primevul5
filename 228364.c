HRESULT Http::HrGetUserAgentVersion(std::string *strUserAgentVersion)
{
	if (m_strUserAgentVersion.empty())
		return MAPI_E_NOT_FOUND;
	strUserAgentVersion -> assign(m_strUserAgentVersion);
	return hrSuccess;
}