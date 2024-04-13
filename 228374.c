HRESULT Http::HrGetDestination(std::string *strDestination)
{
	std::string strHost, strDest;

	// example:  Host: server:port
	auto hr = HrGetHeaderValue("Host", &strHost);
	if(hr != hrSuccess) {
		ec_log_debug("Http::HrGetDestination host header missing");
		return hr;
	}
	// example:  Destination: http://server:port/caldav/username/folderid/entry.ics
	hr = HrGetHeaderValue("Destination", &strDest);
	if (hr != hrSuccess) {
		ec_log_debug("Http::HrGetDestination destination header missing");
		return hr;
	}
	auto pos = strDest.find(strHost);
	if (pos == std::string::npos) {
		ec_log_err("Refusing to move calendar item from %s to different host on url %s", strHost.c_str(), strDest.c_str());
		return MAPI_E_CALL_FAILED;
	}
	strDest.erase(0, pos + strHost.length());
	*strDestination = std::move(strDest);
	return hrSuccess;
}