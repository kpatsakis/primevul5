void ntlm_generate_server_signing_key(NTLM_CONTEXT* context)
{
	SecBuffer signMagic;
	signMagic.pvBuffer = (void*)NTLM_SERVER_SIGN_MAGIC;
	signMagic.cbBuffer = sizeof(NTLM_SERVER_SIGN_MAGIC);
	ntlm_generate_signing_key(context->ExportedSessionKey, &signMagic, context->ServerSigningKey);
}