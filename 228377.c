HRESULT Http::HrGetBody(std::string *strBody)
{
	if (m_strReqBody.empty())
		return MAPI_E_NOT_FOUND;
	strBody->assign(m_strReqBody);
	return hrSuccess;
}