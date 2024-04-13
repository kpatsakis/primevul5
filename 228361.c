HRESULT Http::HrFinalize()
{
	HRESULT hr = hrSuccess;

	HrResponseHeader("Content-Length", stringify(m_strRespBody.length()));

	// force chunked http for long size response, should check version >= 1.1 to disable chunking
	if (m_strRespBody.size() < HTTP_CHUNK_SIZE || m_strHttpVer != "1.1") {
		hr = HrFlushHeaders();
		if (hr != hrSuccess && hr != MAPI_E_END_OF_SESSION) {
			ec_log_debug("Http::HrFinalize flush fail %d", hr);
			m_ulRetCode = 0;
			return hr;
		}
		if (!m_strRespBody.empty()) {
			m_lpChannel->HrWriteString(m_strRespBody);
			ec_log_debug("Response body:\n%s", m_strRespBody.c_str());
		}
	}
	else
	{
		const char *lpstrBody = m_strRespBody.data();
		char lpstrLen[10];
		auto szBodyLen = m_strRespBody.size(); // length of data to be sent to the client
		size_t szBodyWritten = 0; // length of data sent to client
		unsigned int szPart = HTTP_CHUNK_SIZE;						// default length of chunk data to be written

		HrResponseHeader("Transfer-Encoding", "chunked");
		hr = HrFlushHeaders();
		if (hr != hrSuccess && hr != MAPI_E_END_OF_SESSION) {
			ec_log_debug("Http::HrFinalize flush fail(2) %d", hr);
			m_ulRetCode = 0;
			return hr;
		}

		while (szBodyWritten < szBodyLen)
		{
			if ((szBodyWritten + HTTP_CHUNK_SIZE) > szBodyLen)
				szPart = szBodyLen - szBodyWritten;				// change length of data for last chunk
			// send hex length of data and data part
			snprintf(lpstrLen, sizeof(lpstrLen), "%X", szPart);
			m_lpChannel->HrWriteLine(lpstrLen);
			m_lpChannel->HrWriteLine(string_view(lpstrBody, szPart));
			szBodyWritten += szPart;
			lpstrBody += szPart;
		}

		// end of response
		snprintf(lpstrLen, 10, "0\r\n");
		m_lpChannel->HrWriteLine(lpstrLen);
		// just the first part of the body in the log. header shows it's chunked.
		ec_log_debug("%s", m_strRespBody.c_str());
	}

	// if http_log_enable?
	char szTime[32];
	time_t now = time(NULL);
	tm local;
	std::string strAgent;
	localtime_r(&now, &local);
	// @todo we're in C LC_TIME locale to get the correct (month) format, but the timezone will be GMT, which is not wanted.
	strftime(szTime, ARRAY_SIZE(szTime), "%d/%b/%Y:%H:%M:%S %z", &local);
	HrGetHeaderValue("User-Agent", &strAgent);
	ec_log_notice("%s - %s [%s] \"%s\" %d %d \"-\" \"%s\"", m_lpChannel->peer_addr(), m_strUser.empty() ? "-" : m_strUser.c_str(), szTime, m_strAction.c_str(), m_ulRetCode, (int)m_strRespBody.length(), strAgent.c_str());
	m_ulRetCode = 0;
	return hr;
}