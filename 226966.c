static BOOL license_rc4_with_licenseKey(const rdpLicense* license, const BYTE* input, size_t len,
                                        LICENSE_BLOB* target)
{
	WINPR_RC4_CTX* rc4;
	BYTE* buffer = NULL;

	rc4 =
	    winpr_RC4_New_Allow_FIPS(license->LicensingEncryptionKey, LICENSING_ENCRYPTION_KEY_LENGTH);
	if (!rc4)
		return FALSE;

	buffer = (BYTE*)realloc(target->data, len);
	if (!buffer)
		goto error_buffer;

	target->data = buffer;
	target->length = len;

	if (!winpr_RC4_Update(rc4, len, input, buffer))
		goto error_buffer;

	winpr_RC4_Free(rc4);
	return TRUE;

error_buffer:
	winpr_RC4_Free(rc4);
	return FALSE;
}