static BYTE* loadCalFile(rdpSettings* settings, const char* hostname, int* dataLen)
{
	char *licenseStorePath = NULL, *calPath = NULL;
	char calFilename[MAX_PATH];
	char hash[41];
	int length, status;
	FILE* fp;
	BYTE* ret = NULL;

	if (!computeCalHash(hostname, hash))
	{
		WLog_ERR(TAG, "loadCalFile: unable to compute hostname hash");
		return NULL;
	}

	sprintf_s(calFilename, sizeof(calFilename) - 1, "%s.cal", hash);

	if (!(licenseStorePath = GetCombinedPath(settings->ConfigPath, licenseStore)))
		return NULL;

	if (!(calPath = GetCombinedPath(licenseStorePath, calFilename)))
		goto error_path;

	fp = fopen(calPath, "rb");
	if (!fp)
		goto error_open;

	_fseeki64(fp, 0, SEEK_END);
	length = _ftelli64(fp);
	_fseeki64(fp, 0, SEEK_SET);

	ret = (BYTE*)malloc(length);
	if (!ret)
		goto error_malloc;

	status = fread(ret, length, 1, fp);
	if (status <= 0)
		goto error_read;

	*dataLen = length;

	fclose(fp);
	free(calPath);
	free(licenseStorePath);
	return ret;

error_read:
	free(ret);
error_malloc:
	fclose(fp);
error_open:
	free(calPath);
error_path:
	free(licenseStorePath);
	return NULL;
}