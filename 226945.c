BOOL license_send_client_info(rdpLicense* license, const LICENSE_BLOB* calBlob, BYTE* signature)
{
	wStream* s;

	/* Client License Information: */
	UINT32 PlatformId = PLATFORMID;
	UINT32 PreferredKeyExchangeAlg = KEY_EXCHANGE_ALG_RSA;

	s = license_send_stream_init(license);
	if (!s)
		return FALSE;

	Stream_Write_UINT32(s, PreferredKeyExchangeAlg); /* PreferredKeyExchangeAlg (4 bytes) */
	Stream_Write_UINT32(s, PlatformId);              /* PlatformId (4 bytes) */

	/* ClientRandom (32 bytes) */
	Stream_Write(s, license->ClientRandom, CLIENT_RANDOM_LENGTH);

	/* Licensing Binary Blob with EncryptedPreMasterSecret: */
	if (!license_write_encrypted_premaster_secret_blob(s, license->EncryptedPremasterSecret,
	                                                   license->ModulusLength))
		goto error;

	/* Licensing Binary Blob with LicenseInfo: */
	if (!license_write_binary_blob(s, calBlob))
		goto error;

	/* Licensing Binary Blob with EncryptedHWID */
	if (!license_write_binary_blob(s, license->EncryptedHardwareId))
		goto error;

	/* MACData */
	Stream_Write(s, signature, LICENSING_ENCRYPTION_KEY_LENGTH);

	return license_send(license, s, LICENSE_INFO);

error:
	Stream_Release(s);
	return FALSE;
}