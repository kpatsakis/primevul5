BOOL crypto_cert_get_public_key(CryptoCert cert, BYTE** PublicKey, DWORD* PublicKeyLength)
{
	BYTE* ptr;
	int length;
	BOOL status = TRUE;
	EVP_PKEY* pkey = NULL;
	pkey = X509_get_pubkey(cert->px509);

	if (!pkey)
	{
		WLog_ERR(TAG, "X509_get_pubkey() failed");
		status = FALSE;
		goto exit;
	}

	length = i2d_PublicKey(pkey, NULL);

	if (length < 1)
	{
		WLog_ERR(TAG, "i2d_PublicKey() failed");
		status = FALSE;
		goto exit;
	}

	*PublicKeyLength = (DWORD)length;
	*PublicKey = (BYTE*)malloc(length);
	ptr = (BYTE*)(*PublicKey);

	if (!ptr)
	{
		status = FALSE;
		goto exit;
	}

	i2d_PublicKey(pkey, &ptr);
exit:

	if (pkey)
		EVP_PKEY_free(pkey);

	return status;
}