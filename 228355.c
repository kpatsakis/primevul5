HRESULT Http::HrToHTTPCode(HRESULT hr)
{
	if (hr == hrSuccess)
		return HrResponseHeader(200, "Ok");
	else if (hr == MAPI_E_NO_ACCESS)
		return HrResponseHeader(403, "Forbidden");
	else if (hr == MAPI_E_NOT_FOUND)
		return HrResponseHeader(404, "Not Found");
	// @todo other codes?
	return HrResponseHeader(500, "Unhanded error " + stringify_hex(hr));
}