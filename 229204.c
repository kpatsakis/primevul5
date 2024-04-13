void ntlm_generate_client_signing_key(NTLM_CONTEXT* context)
{
	SecBuffer signMagic;
	signMagic.pvBuffer = (void*)NTLM_CLIENT_SIGN_MAGIC;
	signMagic.cbBuffer = sizeof(NTLM_CLIENT_SIGN_MAGIC);
	ntlm_generate_signing_key(context->ExportedSessionKey, &signMagic, context->ClientSigningKey);
}