HRESULT Http::HrResponseHeader(unsigned int ulCode, const std::string &strResponse)
{
	m_ulRetCode = ulCode;
	// do not set headers if once set
	if (!m_strRespHeader.empty())
		return MAPI_E_CALL_FAILED;
	m_strRespHeader = "HTTP/1.1 " + stringify(ulCode) + " " + strResponse;
	return hrSuccess;
}