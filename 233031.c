char* crypto_cert_fingerprint_by_hash(X509* xcert, const char* hash)
{
	UINT32 fp_len, i;
	BYTE* fp;
	char* p;
	char* fp_buffer;

	fp = crypto_cert_hash(xcert, hash, &fp_len);
	if (!fp)
		return NULL;

	fp_buffer = calloc(fp_len * 3 + 1, sizeof(char));
	if (!fp_buffer)
		goto fail;

	p = fp_buffer;

	for (i = 0; i < (fp_len - 1); i++)
	{
		sprintf_s(p, (fp_len - i) * 3, "%02" PRIx8 ":", fp[i]);
		p = &fp_buffer[(i + 1) * 3];
	}

	sprintf_s(p, (fp_len - i) * 3, "%02" PRIx8 "", fp[i]);
fail:
	free(fp);

	return fp_buffer;
}