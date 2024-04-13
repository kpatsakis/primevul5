BOOL license_answer_license_request(rdpLicense* license)
{
	wStream* s;
	BYTE* license_data = NULL;
	int license_size = 0;
	BOOL status;
	char* username;

	if (!license->rdp->settings->OldLicenseBehaviour)
		license_data = loadCalFile(license->rdp->settings, license->rdp->settings->ClientHostname,
		                           &license_size);

	if (license_data)
	{
		LICENSE_BLOB* calBlob = NULL;
		BYTE signature[LICENSING_ENCRYPTION_KEY_LENGTH];

		DEBUG_LICENSE("Sending Saved License Packet");

		license->EncryptedHardwareId->type = BB_ENCRYPTED_DATA_BLOB;
		if (!license_encrypt_and_MAC(license, license->HardwareId, HWID_LENGTH,
		                             license->EncryptedHardwareId, signature))
		{
			free(license_data);
			return FALSE;
		}

		calBlob = license_new_binary_blob(BB_DATA_BLOB);
		if (!calBlob)
		{
			free(license_data);
			return FALSE;
		}
		calBlob->data = license_data;
		calBlob->length = license_size;

		status = license_send_client_info(license, calBlob, signature);
		license_free_binary_blob(calBlob);

		return status;
	}

	DEBUG_LICENSE("Sending New License Packet");

	s = license_send_stream_init(license);
	if (!s)
		return FALSE;
	if (license->rdp->settings->Username != NULL)
		username = license->rdp->settings->Username;
	else
		username = "username";

	license->ClientUserName->data = (BYTE*)username;
	license->ClientUserName->length = strlen(username) + 1;
	license->ClientMachineName->data = (BYTE*)license->rdp->settings->ClientHostname;
	license->ClientMachineName->length = strlen(license->rdp->settings->ClientHostname) + 1;

	status = license_write_new_license_request_packet(license, s);

	license->ClientUserName->data = NULL;
	license->ClientUserName->length = 0;
	license->ClientMachineName->data = NULL;
	license->ClientMachineName->length = 0;

	if (!status)
	{
		Stream_Release(s);
		return FALSE;
	}

	return license_send(license, s, NEW_LICENSE_REQUEST);
}