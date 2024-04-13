HRESULT Http::HrParseHeaders()
{
	std::string strAuthdata;
	std::string strUserAgent;

	auto items = tokenize(m_strAction, ' ', true);
	if (items.size() != 3) {
		ec_log_debug("HrParseHeaders invalid != 3 tokens");
		return MAPI_E_INVALID_PARAMETER;
	}
	m_strMethod = items[0];
	m_strURL = items[1];
	m_strHttpVer = items[2];
	// converts %20 -> ' '
	m_strPath = urlDecode(m_strURL);

	// find the content-type
	// Content-Type: text/xml;charset=UTF-8
	auto hr = HrGetHeaderValue("Content-Type", &m_strCharSet);
	if (hr == hrSuccess)
		m_strCharSet = content_type_get_charset(m_strCharSet.c_str(), m_lpConfig->GetSetting("default_charset"));
	else
		m_strCharSet = m_lpConfig->GetSetting("default_charset"); // really should be UTF-8

	hr = HrGetHeaderValue("User-Agent", &strUserAgent);
	if (hr == hrSuccess) {
		size_t space = strUserAgent.find(" ");

		if (space != std::string::npos) {
			m_strUserAgent = strUserAgent.substr(0, space);
			m_strUserAgentVersion = strUserAgent.substr(space + 1);
		}
		else {
			m_strUserAgent = strUserAgent;
		}
	}

	// find the Authorisation data (Authorization: Basic wr8y273yr2y3r87y23ry7=)
	hr = HrGetHeaderValue("Authorization", &strAuthdata);
	if (hr != hrSuccess) {
		hr = HrGetHeaderValue("WWW-Authenticate", &strAuthdata);
		if (hr != hrSuccess)
			return S_OK; /* ignore empty Authorization */
	}

	items = tokenize(strAuthdata, ' ', true);
	// we only support basic authentication
	if (items.size() != 2 || items[0] != "Basic") {
		ec_log_debug("HrParseHeaders login failed");
		return MAPI_E_LOGON_FAILED;
	}
	auto user_pass = base64_decode(items[1]);
	auto colon_pos = user_pass.find(":");
	if (colon_pos == std::string::npos) {
		ec_log_debug("HrParseHeaders password missing");
		return MAPI_E_LOGON_FAILED;
	}

	m_strUser = user_pass.substr(0, colon_pos);
	m_strPass = user_pass.substr(colon_pos+1, std::string::npos);
	return hrSuccess;
}