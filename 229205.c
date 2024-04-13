void ntlm_compute_message_integrity_check(NTLM_CONTEXT* context, BYTE* mic, UINT32 size)
{
	/*
	 * Compute the HMAC-MD5 hash of ConcatenationOf(NEGOTIATE_MESSAGE,
	 * CHALLENGE_MESSAGE, AUTHENTICATE_MESSAGE) using the ExportedSessionKey
	 */
	WINPR_HMAC_CTX* hmac = winpr_HMAC_New();
	assert(size >= WINPR_MD5_DIGEST_LENGTH);

	if (!hmac)
		return;

	if (winpr_HMAC_Init(hmac, WINPR_MD_MD5, context->ExportedSessionKey, WINPR_MD5_DIGEST_LENGTH))
	{
		winpr_HMAC_Update(hmac, (BYTE*)context->NegotiateMessage.pvBuffer,
		                  context->NegotiateMessage.cbBuffer);
		winpr_HMAC_Update(hmac, (BYTE*)context->ChallengeMessage.pvBuffer,
		                  context->ChallengeMessage.cbBuffer);
		winpr_HMAC_Update(hmac, (BYTE*)context->AuthenticateMessage.pvBuffer,
		                  context->AuthenticateMessage.cbBuffer);
		winpr_HMAC_Final(hmac, mic, WINPR_MD5_DIGEST_LENGTH);
	}

	winpr_HMAC_Free(hmac);
}