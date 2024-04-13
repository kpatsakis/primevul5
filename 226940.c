static BOOL computeCalHash(const char* hostname, char* hashStr)
{
	WINPR_DIGEST_CTX* sha1 = NULL;
	BOOL ret = FALSE;
	BYTE hash[20];
	size_t i;

	if (!(sha1 = winpr_Digest_New()))
		goto out;
	if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1))
		goto out;
	if (!winpr_Digest_Update(sha1, (const BYTE*)hostname, strlen(hostname)))
		goto out;
	if (!winpr_Digest_Final(sha1, hash, sizeof(hash)))
		goto out;

	for (i = 0; i < sizeof(hash); i++, hashStr += 2)
		sprintf_s(hashStr, 3, "%.2x", hash[i]);

	ret = TRUE;
out:
	winpr_Digest_Free(sha1);
	return ret;
}