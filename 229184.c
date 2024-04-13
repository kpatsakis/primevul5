void ntlm_init_rc4_seal_states(NTLM_CONTEXT* context)
{
	if (context->server)
	{
		context->SendSigningKey = context->ServerSigningKey;
		context->RecvSigningKey = context->ClientSigningKey;
		context->SendSealingKey = context->ClientSealingKey;
		context->RecvSealingKey = context->ServerSealingKey;
		context->SendRc4Seal = winpr_RC4_New(context->ServerSealingKey, 16);
		context->RecvRc4Seal = winpr_RC4_New(context->ClientSealingKey, 16);
	}
	else
	{
		context->SendSigningKey = context->ClientSigningKey;
		context->RecvSigningKey = context->ServerSigningKey;
		context->SendSealingKey = context->ServerSealingKey;
		context->RecvSealingKey = context->ClientSealingKey;
		context->SendRc4Seal = winpr_RC4_New(context->ClientSealingKey, 16);
		context->RecvRc4Seal = winpr_RC4_New(context->ServerSealingKey, 16);
	}
}