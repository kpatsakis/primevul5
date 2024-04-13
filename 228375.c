HRESULT Http::HrResponseBody(const std::string &strResponse)
{
	m_strRespBody += strResponse;
	// data send in HrFinalize()
	return hrSuccess;
}