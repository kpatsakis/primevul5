rdpLicense* license_new(rdpRdp* rdp)
{
	rdpLicense* license;
	license = (rdpLicense*)calloc(1, sizeof(rdpLicense));
	if (!license)
		return NULL;

	license->rdp = rdp;
	license->state = LICENSE_STATE_AWAIT;
	if (!(license->certificate = certificate_new()))
		goto out_error;
	if (!(license->ProductInfo = license_new_product_info()))
		goto out_error;
	if (!(license->ErrorInfo = license_new_binary_blob(BB_ERROR_BLOB)))
		goto out_error;
	if (!(license->KeyExchangeList = license_new_binary_blob(BB_KEY_EXCHG_ALG_BLOB)))
		goto out_error;
	if (!(license->ServerCertificate = license_new_binary_blob(BB_CERTIFICATE_BLOB)))
		goto out_error;
	if (!(license->ClientUserName = license_new_binary_blob(BB_CLIENT_USER_NAME_BLOB)))
		goto out_error;
	if (!(license->ClientMachineName = license_new_binary_blob(BB_CLIENT_MACHINE_NAME_BLOB)))
		goto out_error;
	if (!(license->PlatformChallenge = license_new_binary_blob(BB_ANY_BLOB)))
		goto out_error;
	if (!(license->EncryptedPlatformChallenge = license_new_binary_blob(BB_ANY_BLOB)))
		goto out_error;
	if (!(license->EncryptedPlatformChallengeResponse =
	          license_new_binary_blob(BB_ENCRYPTED_DATA_BLOB)))
		goto out_error;
	if (!(license->EncryptedPremasterSecret = license_new_binary_blob(BB_ANY_BLOB)))
		goto out_error;
	if (!(license->EncryptedHardwareId = license_new_binary_blob(BB_ENCRYPTED_DATA_BLOB)))
		goto out_error;
	if (!(license->ScopeList = license_new_scope_list()))
		goto out_error;

	license_generate_randoms(license);

	return license;

out_error:
	license_free(license);
	return NULL;
}