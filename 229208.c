static int ntlm_compute_ntlm_v2_hash(NTLM_CONTEXT* context, BYTE* hash)
{
	SSPI_CREDENTIALS* credentials = context->credentials;
#ifdef WITH_DEBUG_NTLM

	if (credentials)
	{
		WLog_DBG(TAG, "Password (length = %" PRIu32 ")", credentials->identity.PasswordLength * 2);
		winpr_HexDump(TAG, WLOG_DEBUG, (BYTE*)credentials->identity.Password,
		              credentials->identity.PasswordLength * 2);
		WLog_DBG(TAG, "Username (length = %" PRIu32 ")", credentials->identity.UserLength * 2);
		winpr_HexDump(TAG, WLOG_DEBUG, (BYTE*)credentials->identity.User,
		              credentials->identity.UserLength * 2);
		WLog_DBG(TAG, "Domain (length = %" PRIu32 ")", credentials->identity.DomainLength * 2);
		winpr_HexDump(TAG, WLOG_DEBUG, (BYTE*)credentials->identity.Domain,
		              credentials->identity.DomainLength * 2);
	}
	else
		WLog_DBG(TAG, "Strange, NTLM_CONTEXT is missing valid credentials...");

	WLog_DBG(TAG, "Workstation (length = %" PRIu16 ")", context->Workstation.Length);
	winpr_HexDump(TAG, WLOG_DEBUG, (BYTE*)context->Workstation.Buffer, context->Workstation.Length);
	WLog_DBG(TAG, "NTOWFv2, NTLMv2 Hash");
	winpr_HexDump(TAG, WLOG_DEBUG, context->NtlmV2Hash, WINPR_MD5_DIGEST_LENGTH);
#endif

	if (memcmp(context->NtlmV2Hash, NTLM_NULL_BUFFER, 16) != 0)
		return 1;

	if (!credentials)
		return -1;
	else if (memcmp(context->NtlmHash, NTLM_NULL_BUFFER, 16) != 0)
	{
		NTOWFv2FromHashW(context->NtlmHash, (LPWSTR)credentials->identity.User,
		                 credentials->identity.UserLength * 2, (LPWSTR)credentials->identity.Domain,
		                 credentials->identity.DomainLength * 2, (BYTE*)hash);
	}
	else if (credentials->identity.PasswordLength > SSPI_CREDENTIALS_HASH_LENGTH_OFFSET)
	{
		/* Special case for WinPR: password hash */
		if (ntlm_convert_password_hash(context, context->NtlmHash) < 0)
			return -1;

		NTOWFv2FromHashW(context->NtlmHash, (LPWSTR)credentials->identity.User,
		                 credentials->identity.UserLength * 2, (LPWSTR)credentials->identity.Domain,
		                 credentials->identity.DomainLength * 2, (BYTE*)hash);
	}
	else if (credentials->identity.Password)
	{
		NTOWFv2W((LPWSTR)credentials->identity.Password, credentials->identity.PasswordLength * 2,
		         (LPWSTR)credentials->identity.User, credentials->identity.UserLength * 2,
		         (LPWSTR)credentials->identity.Domain, credentials->identity.DomainLength * 2,
		         (BYTE*)hash);
	}
	else if (context->HashCallback)
	{
		int ret;
		SecBuffer proofValue, micValue;

		if (ntlm_computeProofValue(context, &proofValue) != SEC_E_OK)
			return -1;

		if (ntlm_computeMicValue(context, &micValue) != SEC_E_OK)
		{
			sspi_SecBufferFree(&proofValue);
			return -1;
		}

		ret = context->HashCallback(context->HashCallbackArg, &credentials->identity, &proofValue,
		                            context->EncryptedRandomSessionKey,
		                            (&context->AUTHENTICATE_MESSAGE)->MessageIntegrityCheck,
		                            &micValue, hash);
		sspi_SecBufferFree(&proofValue);
		sspi_SecBufferFree(&micValue);
		return ret ? 1 : -1;
	}
	else if (context->UseSamFileDatabase)
	{
		return ntlm_fetch_ntlm_v2_hash(context, hash);
	}

	return 1;
}