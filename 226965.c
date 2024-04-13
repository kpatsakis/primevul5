static BOOL saveCal(rdpSettings* settings, const BYTE* data, int length, char* hostname)
{
	char hash[41];
	FILE* fp;
	char* licenseStorePath = NULL;
	char filename[MAX_PATH], filenameNew[MAX_PATH];
	char *filepath = NULL, *filepathNew = NULL;
	size_t written;
	BOOL ret = FALSE;

	if (!PathFileExistsA(settings->ConfigPath))
	{
		if (!PathMakePathA(settings->ConfigPath, 0))
		{
			WLog_ERR(TAG, "error creating directory '%s'", settings->ConfigPath);
			goto out;
		}
		WLog_INFO(TAG, "creating directory %s", settings->ConfigPath);
	}

	if (!(licenseStorePath = GetCombinedPath(settings->ConfigPath, licenseStore)))
		goto out;

	if (!PathFileExistsA(licenseStorePath))
	{
		if (!PathMakePathA(licenseStorePath, 0))
		{
			WLog_ERR(TAG, "error creating directory '%s'", licenseStorePath);
			goto out;
		}
		WLog_INFO(TAG, "creating directory %s", licenseStorePath);
	}

	if (!computeCalHash(hostname, hash))
		goto out;
	sprintf_s(filename, sizeof(filename) - 1, "%s.cal", hash);
	sprintf_s(filenameNew, sizeof(filenameNew) - 1, "%s.cal.new", hash);

	if (!(filepath = GetCombinedPath(licenseStorePath, filename)))
		goto out;

	if (!(filepathNew = GetCombinedPath(licenseStorePath, filenameNew)))
		goto out;

	fp = fopen(filepathNew, "wb");
	if (!fp)
		goto out;

	written = fwrite(data, length, 1, fp);
	fclose(fp);

	if (written != 1)
	{
		DeleteFile(filepathNew);
		goto out;
	}

	ret = MoveFileEx(filepathNew, filepath, MOVEFILE_REPLACE_EXISTING);

out:
	free(filepathNew);
	free(filepath);
	free(licenseStorePath);
	return ret;
}