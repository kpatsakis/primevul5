void CServer::ConchainSpecialInfoupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	pfnCallback(pResult, pCallbackUserData);
	CServer *pSelf = (CServer *)pUserData;
	if(pResult->NumArguments())
	{
		str_clean_whitespaces(pSelf->Config()->m_SvName);
		pSelf->SendServerInfo(-1);
	}
}