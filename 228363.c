HRESULT Http::HrRequestAuth(const std::string &strMsg)
{
	auto hr = HrResponseHeader(401, "Unauthorized");
	if (hr != hrSuccess)
		return hr;
	return HrResponseHeader("WWW-Authenticate", "Basic realm=\"" + strMsg + "\"");
}