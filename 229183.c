void ntlm_generate_server_sealing_key(NTLM_CONTEXT* context)
{
	SecBuffer sealMagic;
	sealMagic.pvBuffer = (void*)NTLM_SERVER_SEAL_MAGIC;
	sealMagic.cbBuffer = sizeof(NTLM_SERVER_SEAL_MAGIC);
	ntlm_generate_signing_key(context->ExportedSessionKey, &sealMagic, context->ServerSealingKey);
}