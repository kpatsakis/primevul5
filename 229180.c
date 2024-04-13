static int ntlm_generate_signing_key(BYTE* exported_session_key, PSecBuffer sign_magic,
                                     BYTE* signing_key)
{
	int length;
	BYTE* value;
	length = WINPR_MD5_DIGEST_LENGTH + sign_magic->cbBuffer;
	value = (BYTE*)malloc(length);

	if (!value)
		return -1;

	/* Concatenate ExportedSessionKey with sign magic */
	CopyMemory(value, exported_session_key, WINPR_MD5_DIGEST_LENGTH);
	CopyMemory(&value[WINPR_MD5_DIGEST_LENGTH], sign_magic->pvBuffer, sign_magic->cbBuffer);

	if (!winpr_Digest(WINPR_MD_MD5, value, length, signing_key, WINPR_MD5_DIGEST_LENGTH))
	{
		free(value);
		return -1;
	}

	free(value);
	return 1;
}