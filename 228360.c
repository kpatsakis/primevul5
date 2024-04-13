HRESULT Http::HrGetCharSet(std::string *strCharset)
{
	if (m_strCharSet.empty())
		return MAPI_E_NOT_FOUND;
	strCharset->assign(m_strCharSet);
	return hrSuccess;
}