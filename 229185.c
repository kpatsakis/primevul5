static int ntlm_fetch_ntlm_v2_hash(NTLM_CONTEXT* context, BYTE* hash)
{
	WINPR_SAM* sam;
	WINPR_SAM_ENTRY* entry;
	SSPI_CREDENTIALS* credentials = context->credentials;
	sam = SamOpen(context->SamFile, TRUE);

	if (!sam)
		return -1;

	entry = SamLookupUserW(
	    sam, (LPWSTR)credentials->identity.User, credentials->identity.UserLength * 2,
	    (LPWSTR)credentials->identity.Domain, credentials->identity.DomainLength * 2);

	if (entry)
	{
#ifdef WITH_DEBUG_NTLM
		WLog_DBG(TAG, "NTLM Hash:");
		winpr_HexDump(TAG, WLOG_DEBUG, entry->NtHash, 16);
#endif
		NTOWFv2FromHashW(entry->NtHash, (LPWSTR)credentials->identity.User,
		                 credentials->identity.UserLength * 2, (LPWSTR)credentials->identity.Domain,
		                 credentials->identity.DomainLength * 2, (BYTE*)hash);
		SamFreeEntry(sam, entry);
		SamClose(sam);
		return 1;
	}

	entry = SamLookupUserW(sam, (LPWSTR)credentials->identity.User,
	                       credentials->identity.UserLength * 2, NULL, 0);

	if (entry)
	{
#ifdef WITH_DEBUG_NTLM
		WLog_DBG(TAG, "NTLM Hash:");
		winpr_HexDump(TAG, WLOG_DEBUG, entry->NtHash, 16);
#endif
		NTOWFv2FromHashW(entry->NtHash, (LPWSTR)credentials->identity.User,
		                 credentials->identity.UserLength * 2, (LPWSTR)credentials->identity.Domain,
		                 credentials->identity.DomainLength * 2, (BYTE*)hash);
		SamFreeEntry(sam, entry);
		SamClose(sam);
		return 1;
	}
	else
	{
		SamClose(sam);
		WLog_ERR(TAG, "Error: Could not find user in SAM database");
		return 0;
	}

	SamClose(sam);
	return 1;
}