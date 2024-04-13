HRESULT Http::HrResponseHeader(const std::string &strHeader, const std::string &strValue)
{
	m_lstHeaders.emplace_back(strHeader + ": " + strValue);
	return hrSuccess;
}