void license_free_binary_blob(LICENSE_BLOB* blob)
{
	if (blob)
	{
		free(blob->data);
		free(blob);
	}
}