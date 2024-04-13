static BOOL license_write_encrypted_premaster_secret_blob(wStream* s, const LICENSE_BLOB* blob,
                                                          UINT32 ModulusLength)
{
	UINT32 length;
	length = ModulusLength + 8;

	if (blob->length > ModulusLength)
	{
		WLog_ERR(TAG, "license_write_encrypted_premaster_secret_blob: invalid blob");
		return FALSE;
	}

	if (!Stream_EnsureRemainingCapacity(s, length + 4))
		return FALSE;
	Stream_Write_UINT16(s, blob->type); /* wBlobType (2 bytes) */
	Stream_Write_UINT16(s, length);     /* wBlobLen (2 bytes) */

	if (blob->length > 0)
		Stream_Write(s, blob->data, blob->length); /* blobData */

	Stream_Zero(s, length - blob->length);
	return TRUE;
}