HRESULT Http::HrGetHeaderValue(const std::string &strHeader, std::string *strValue)
{
	auto iHeader = mapHeaders.find(strHeader);
	if (iHeader == mapHeaders.cend())
		return MAPI_E_NOT_FOUND;
	*strValue = iHeader->second;
	return hrSuccess;
}