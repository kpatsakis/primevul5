HRESULT Http::HrGetMethod(std::string *strMethod)
{
	if (m_strMethod.empty())
		return MAPI_E_NOT_FOUND;
	strMethod->assign(m_strMethod);
	return hrSuccess;
}