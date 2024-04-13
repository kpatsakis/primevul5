BYTE* crypto_cert_hash(X509* xcert, const char* hash, UINT32* length)
{
	UINT32 fp_len = EVP_MAX_MD_SIZE;
	BYTE* fp;
	const EVP_MD* md = EVP_get_digestbyname(hash);
	if (!md)
		return NULL;
	if (!length)
		return NULL;
	if (!xcert)
		return NULL;

	fp = calloc(fp_len, sizeof(BYTE));
	if (!fp)
		return NULL;

	if (X509_digest(xcert, md, fp, &fp_len) != 1)
	{
		free(fp);
		return NULL;
	}

	*length = fp_len;
	return fp;
}