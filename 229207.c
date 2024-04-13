void ntlm_generate_server_challenge(NTLM_CONTEXT* context)
{
	if (memcmp(context->ServerChallenge, NTLM_NULL_BUFFER, 8) == 0)
		winpr_RAND(context->ServerChallenge, 8);
}