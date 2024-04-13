HRESULT Http::HrSetKeepAlive(int ulKeepAlive)
{
	m_ulKeepAlive = ulKeepAlive;
	return hrSuccess;
}