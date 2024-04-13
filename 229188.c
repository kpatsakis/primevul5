static int ntlm_convert_password_hash(NTLM_CONTEXT* context, BYTE* hash)
{
	int status;
	int i, hn, ln;
	char* PasswordHash = NULL;
	UINT32 PasswordHashLength = 0;
	SSPI_CREDENTIALS* credentials = context->credentials;
	/* Password contains a password hash of length (PasswordLength -
	 * SSPI_CREDENTIALS_HASH_LENGTH_OFFSET) */
	PasswordHashLength = credentials->identity.PasswordLength - SSPI_CREDENTIALS_HASH_LENGTH_OFFSET;
	status = ConvertFromUnicode(CP_UTF8, 0, (LPCWSTR)credentials->identity.Password,
	                            PasswordHashLength, &PasswordHash, 0, NULL, NULL);

	if (status <= 0)
		return -1;

	CharUpperBuffA(PasswordHash, PasswordHashLength);

	for (i = 0; i < 32; i += 2)
	{
		hn = PasswordHash[i] > '9' ? PasswordHash[i] - 'A' + 10 : PasswordHash[i] - '0';
		ln = PasswordHash[i + 1] > '9' ? PasswordHash[i + 1] - 'A' + 10 : PasswordHash[i + 1] - '0';
		hash[i / 2] = (hn << 4) | ln;
	}

	free(PasswordHash);
	return 1;
}