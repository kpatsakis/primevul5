static int ntlm_generate_sealing_key(BYTE* exported_session_key, PSecBuffer seal_magic,
                                     BYTE* sealing_key)
{
	BYTE* p;
	SecBuffer buffer;

	if (!sspi_SecBufferAlloc(&buffer, WINPR_MD5_DIGEST_LENGTH + seal_magic->cbBuffer))
		return -1;

	p = (BYTE*)buffer.pvBuffer;
	/* Concatenate ExportedSessionKey with seal magic */
	CopyMemory(p, exported_session_key, WINPR_MD5_DIGEST_LENGTH);
	CopyMemory(&p[WINPR_MD5_DIGEST_LENGTH], seal_magic->pvBuffer, seal_magic->cbBuffer);

	if (!winpr_Digest(WINPR_MD_MD5, buffer.pvBuffer, buffer.cbBuffer, sealing_key,
	                  WINPR_MD5_DIGEST_LENGTH))
	{
		sspi_SecBufferFree(&buffer);
		return -1;
	}

	sspi_SecBufferFree(&buffer);
	return 1;
}