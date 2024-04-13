HRESULT Http::X2W(const std::string &strIn, std::wstring *lpstrOut)
{
	const char *lpszCharset = (m_strCharSet.empty() ? "UTF-8" : m_strCharSet.c_str());
	return TryConvert(m_converter, strIn, rawsize(strIn), lpszCharset, *lpstrOut);
}