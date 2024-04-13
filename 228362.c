HRESULT Http::HrGetPass(std::wstring *strPass)
{
	if (m_strPass.empty())
		return MAPI_E_NOT_FOUND;
	return X2W(m_strPass, strPass);
}