HRESULT Http::HrFlushHeaders()
{
	HRESULT hr = hrSuccess;
	std::string strOutput, strConnection;
	char lpszChar[128];

	HrGetHeaderValue("Connection", &strConnection);
	// Add misc. headers
	HrResponseHeader("Server","Kopano");
	struct tm dummy;
	strftime(lpszChar, 127, "%a, %d %b %Y %H:%M:%S GMT", gmtime_safe(time(nullptr), &dummy));
	HrResponseHeader("Date", lpszChar);
	if (m_ulKeepAlive != 0 && strcasecmp(strConnection.c_str(), "keep-alive") == 0) {
		HrResponseHeader("Connection", "Keep-Alive");
		HrResponseHeader("Keep-Alive", stringify(m_ulKeepAlive));
	}
	else
	{
		HrResponseHeader("Connection", "close");
		hr = MAPI_E_END_OF_SESSION;
	}

	// create headers packet
	assert(m_ulRetCode != 0);
	if (m_ulRetCode == 0)
		HrResponseHeader(500, "Request handled incorrectly");
	ec_log_debug("> " + m_strRespHeader);
	strOutput += m_strRespHeader + "\r\n";
	m_strRespHeader.clear();
	for (const auto &h : m_lstHeaders) {
		ec_log_debug("> " + h);
		strOutput += h + "\r\n";
	}
	m_lstHeaders.clear();
	//as last line has a CRLF. The HrWriteLine adds one more CRLF.
	//this means the End of headder.
	m_lpChannel->HrWriteLine(strOutput);
	return hr;
}