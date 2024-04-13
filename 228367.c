HRESULT Http::HrGetUrl(std::string *strUrl)
{
	if (m_strPath.empty())
		return MAPI_E_NOT_FOUND;
	strUrl->assign(urlDecode(m_strPath));
	return hrSuccess;
}