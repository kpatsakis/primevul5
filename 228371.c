HRESULT Http::HrReadBody()
{
	std::string strLength;

	// find the Content-Length
	if (HrGetHeaderValue("Content-Length", &strLength) != hrSuccess) {
		ec_log_debug("Http::HrReadBody content-length missing");
		return MAPI_E_NOT_FOUND;
	}
	auto ulContLength = atoi(strLength.c_str());
	if (ulContLength <= 0) {
		ec_log_debug("Http::HrReadBody content-length invalid %d", ulContLength);
		return MAPI_E_NOT_FOUND;
	}
	auto hr = m_lpChannel->HrReadBytes(&m_strReqBody, ulContLength);
	if (!m_strUser.empty())
		ec_log_debug("Request body:\n%s", m_strReqBody.c_str());
	return hr;
}