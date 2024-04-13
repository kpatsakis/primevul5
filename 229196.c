int ntlm_compute_ntlm_v2_response(NTLM_CONTEXT* context)
{
	BYTE* blob;
	SecBuffer ntlm_v2_temp = { 0 };
	SecBuffer ntlm_v2_temp_chal = { 0 };
	PSecBuffer TargetInfo = &context->ChallengeTargetInfo;
	int ret = -1;

	if (!sspi_SecBufferAlloc(&ntlm_v2_temp, TargetInfo->cbBuffer + 28))
		goto exit;

	ZeroMemory(ntlm_v2_temp.pvBuffer, ntlm_v2_temp.cbBuffer);
	blob = (BYTE*)ntlm_v2_temp.pvBuffer;

	/* Compute the NTLMv2 hash */
	if (ntlm_compute_ntlm_v2_hash(context, (BYTE*)context->NtlmV2Hash) < 0)
		goto exit;

	/* Construct temp */
	blob[0] = 1; /* RespType (1 byte) */
	blob[1] = 1; /* HighRespType (1 byte) */
	/* Reserved1 (2 bytes) */
	/* Reserved2 (4 bytes) */
	CopyMemory(&blob[8], context->Timestamp, 8);        /* Timestamp (8 bytes) */
	CopyMemory(&blob[16], context->ClientChallenge, 8); /* ClientChallenge (8 bytes) */
	/* Reserved3 (4 bytes) */
	CopyMemory(&blob[28], TargetInfo->pvBuffer, TargetInfo->cbBuffer);
#ifdef WITH_DEBUG_NTLM
	WLog_DBG(TAG, "NTLMv2 Response Temp Blob");
	winpr_HexDump(TAG, WLOG_DEBUG, ntlm_v2_temp.pvBuffer, ntlm_v2_temp.cbBuffer);
#endif

	/* Concatenate server challenge with temp */

	if (!sspi_SecBufferAlloc(&ntlm_v2_temp_chal, ntlm_v2_temp.cbBuffer + 8))
		goto exit;

	blob = (BYTE*)ntlm_v2_temp_chal.pvBuffer;
	CopyMemory(blob, context->ServerChallenge, 8);
	CopyMemory(&blob[8], ntlm_v2_temp.pvBuffer, ntlm_v2_temp.cbBuffer);
	winpr_HMAC(WINPR_MD_MD5, (BYTE*)context->NtlmV2Hash, WINPR_MD5_DIGEST_LENGTH,
	           (BYTE*)ntlm_v2_temp_chal.pvBuffer, ntlm_v2_temp_chal.cbBuffer,
	           context->NtProofString, WINPR_MD5_DIGEST_LENGTH);

	/* NtChallengeResponse, Concatenate NTProofStr with temp */

	if (!sspi_SecBufferAlloc(&context->NtChallengeResponse, ntlm_v2_temp.cbBuffer + 16))
		goto exit;

	blob = (BYTE*)context->NtChallengeResponse.pvBuffer;
	CopyMemory(blob, context->NtProofString, WINPR_MD5_DIGEST_LENGTH);
	CopyMemory(&blob[16], ntlm_v2_temp.pvBuffer, ntlm_v2_temp.cbBuffer);
	/* Compute SessionBaseKey, the HMAC-MD5 hash of NTProofStr using the NTLMv2 hash as the key */
	winpr_HMAC(WINPR_MD_MD5, (BYTE*)context->NtlmV2Hash, WINPR_MD5_DIGEST_LENGTH,
	           context->NtProofString, WINPR_MD5_DIGEST_LENGTH, context->SessionBaseKey,
	           WINPR_MD5_DIGEST_LENGTH);
	ret = 1;
exit:
	sspi_SecBufferFree(&ntlm_v2_temp);
	sspi_SecBufferFree(&ntlm_v2_temp_chal);
	return ret;
}