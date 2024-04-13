void CServer::ConchainPlayerSlotsUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	pfnCallback(pResult, pCallbackUserData);
	CServer *pSelf = (CServer *)pUserData;
	if(pResult->NumArguments())
	{
		if(pSelf->Config()->m_SvMaxClients < pSelf->Config()->m_SvPlayerSlots)
			pSelf->Config()->m_SvPlayerSlots = pSelf->Config()->m_SvMaxClients;
	}
}