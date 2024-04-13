void license_free(rdpLicense* license)
{
	if (license)
	{
		free(license->Modulus);
		certificate_free(license->certificate);
		license_free_product_info(license->ProductInfo);
		license_free_binary_blob(license->ErrorInfo);
		license_free_binary_blob(license->KeyExchangeList);
		license_free_binary_blob(license->ServerCertificate);
		license_free_binary_blob(license->ClientUserName);
		license_free_binary_blob(license->ClientMachineName);
		license_free_binary_blob(license->PlatformChallenge);
		license_free_binary_blob(license->EncryptedPlatformChallenge);
		license_free_binary_blob(license->EncryptedPlatformChallengeResponse);
		license_free_binary_blob(license->EncryptedPremasterSecret);
		license_free_binary_blob(license->EncryptedHardwareId);
		license_free_scope_list(license->ScopeList);
		free(license);
	}
}