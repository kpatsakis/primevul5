LICENSE_BLOB* license_new_binary_blob(UINT16 type)
{
	LICENSE_BLOB* blob;
	blob = (LICENSE_BLOB*)calloc(1, sizeof(LICENSE_BLOB));
	if (blob)
		blob->type = type;
	return blob;
}