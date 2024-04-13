void ntlm_generate_client_sealing_key(NTLM_CONTEXT* context)
{
	SecBuffer sealMagic;
	sealMagic.pvBuffer = (void*)NTLM_CLIENT_SEAL_MAGIC;
	sealMagic.cbBuffer = sizeof(NTLM_CLIENT_SEAL_MAGIC);
	ntlm_generate_signing_key(context->ExportedSessionKey, &sealMagic, context->ClientSealingKey);
}