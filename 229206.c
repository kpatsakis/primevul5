void ntlm_generate_random_session_key(NTLM_CONTEXT* context)
{
	winpr_RAND(context->RandomSessionKey, 16);
}