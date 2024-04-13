void ntlm_generate_client_challenge(NTLM_CONTEXT* context)
{
	/* ClientChallenge is used in computation of LMv2 and NTLMv2 responses */
	if (memcmp(context->ClientChallenge, NTLM_NULL_BUFFER, 8) == 0)
		winpr_RAND(context->ClientChallenge, 8);
}