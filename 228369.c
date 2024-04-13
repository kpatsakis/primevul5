HRESULT Http::HrValidateReq()
{
	static const char *const lpszMethods[] = {
		"ACL", "GET", "HEAD", "POST", "PUT", "DELETE", "OPTIONS",
		"PROPFIND", "REPORT", "MKCALENDAR", "PROPPATCH", "MOVE", NULL,
	};
	bool bFound = false;

	if (m_strMethod.empty())
		return kc_perror("HTTP request method is empty", MAPI_E_INVALID_PARAMETER);
	if (!parseBool(m_lpConfig->GetSetting("enable_ical_get")) && m_strMethod == "GET") {
		ec_log_err("Denying iCalendar GET since it is disabled");
		return MAPI_E_NO_ACCESS;
	}
	for (unsigned int i = 0; lpszMethods[i] != nullptr; ++i) {
		if (m_strMethod == lpszMethods[i]) {
			bFound = true;
			break;
		}
	}

	if (!bFound)
		return hr_lerr(MAPI_E_INVALID_PARAMETER, "HTTP request \"%s\" not implemented", m_strMethod.c_str());
	// validate authentication data
	if (m_strUser.empty() || m_strPass.empty())
		// hr still success, since http request is valid
		ec_log_debug("Request missing authorization data");
	return hrSuccess;
}