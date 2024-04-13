void ntlm_decrypt_random_session_key(NTLM_CONTEXT* context)
{
	/* In NTLMv2, EncryptedRandomSessionKey is the ExportedSessionKey RC4-encrypted with the
	 * KeyExchangeKey */

	/**
	 * 	if (NegotiateFlags & NTLMSSP_NEGOTIATE_KEY_EXCH)
	 * 		Set RandomSessionKey to RC4K(KeyExchangeKey,
	 * AUTHENTICATE_MESSAGE.EncryptedRandomSessionKey) else Set RandomSessionKey to KeyExchangeKey
	 */
	if (context->NegotiateKeyExchange)
		ntlm_rc4k(context->KeyExchangeKey, 16, context->EncryptedRandomSessionKey,
		          context->RandomSessionKey);
	else
		CopyMemory(context->RandomSessionKey, context->KeyExchangeKey, 16);
}