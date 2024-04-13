static BOOL license_read_encrypted_blob(const rdpLicense* license, wStream* s, LICENSE_BLOB* target)
{
	UINT16 wBlobType, wBlobLen;
	BYTE* encryptedData;

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT16(s, wBlobType);
	if (wBlobType != BB_ENCRYPTED_DATA_BLOB)
	{
		WLog_DBG(
		    TAG,
		    "expecting BB_ENCRYPTED_DATA_BLOB blob, probably a windows 2003 server, continuing...");
	}

	Stream_Read_UINT16(s, wBlobLen);

	if (Stream_GetRemainingLength(s) < wBlobLen)
		return FALSE;

	encryptedData = Stream_Pointer(s);
	Stream_Seek(s, wBlobLen);
	return license_rc4_with_licenseKey(license, encryptedData, wBlobLen, target);
}