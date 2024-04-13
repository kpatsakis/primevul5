HRESULT Http::HrGetRequestUrl(std::string *strURL)
{
	strURL->assign(m_strURL);
	return hrSuccess;
}