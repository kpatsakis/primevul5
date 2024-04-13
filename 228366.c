HRESULT Http::HrGetUser(std::wstring *strUser)
{
	if (m_strUser.empty())
		return MAPI_E_NOT_FOUND;
	return X2W(m_strUser, strUser);
}