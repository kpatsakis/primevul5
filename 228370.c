HRESULT HrParseURL(const std::string &strUrl, ULONG *lpulFlag, std::string *lpstrUrlUser, std::string *lpstrFolder)
{
	HRESULT hr = hrSuccess;
	std::string strService, strFolder, strUrlUser;
	ULONG ulFlag = 0;
	auto vcUrlTokens = tokenize(strUrl, L'/', true);
	decltype(vcUrlTokens)::const_iterator iterToken;
	if (vcUrlTokens.empty())
		// root should be present, no flags are set. mostly used on OPTIONS command
		goto exit;

	if (vcUrlTokens.back().rfind(".ics") != std::string::npos)
		// Guid is retrieved using StripGuid().
		vcUrlTokens.pop_back();
	else
		// request is for folder not a calendar entry
		ulFlag |= REQ_COLLECTION;
	if (vcUrlTokens.empty())
		goto exit;
	if (vcUrlTokens.size() > 3) {
		// sub folders are not allowed
		hr = MAPI_E_TOO_COMPLEX;
		goto exit;
	}

	iterToken = vcUrlTokens.cbegin();
	//change case of Service name ICAL -> ical CALDaV ->caldav
	strService = strToLower(*iterToken++);
	if (strService == "ical")
		ulFlag |= SERVICE_ICAL;
	else if (strService == "caldav")
		ulFlag |= SERVICE_CALDAV;
	else
		ulFlag |= SERVICE_UNKNOWN;
	if (iterToken == vcUrlTokens.cend())
		goto exit;

	//change case of folder owner USER -> user, UseR -> user
	strUrlUser = strToLower(*iterToken++);
	// check if the request is for public folders and set the bool flag
	// @note: request for public folder not have user's name in the url
	if (strUrlUser == "public")
		ulFlag |= REQ_PUBLIC;
	if (iterToken == vcUrlTokens.cend())
		goto exit;
	// @todo subfolder/folder/ is not allowed! only subfolder/item.ics
	for (; iterToken != vcUrlTokens.end(); ++iterToken)
		strFolder = strFolder + *iterToken + "/";
	strFolder.erase(strFolder.length() - 1);
exit:
	if (lpulFlag)
		*lpulFlag = ulFlag;
	if (lpstrUrlUser)
		*lpstrUrlUser = std::move(strUrlUser);
	if (lpstrFolder)
		*lpstrFolder = std::move(strFolder);
	return hr;
}