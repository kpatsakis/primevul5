BOOL license_encrypt_premaster_secret(rdpLicense* license)
{
	BYTE* EncryptedPremasterSecret;

	if (!license_get_server_rsa_public_key(license))
		return FALSE;

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "Modulus (%" PRIu32 " bits):", license->ModulusLength * 8);
	winpr_HexDump(TAG, WLOG_DEBUG, license->Modulus, license->ModulusLength);
	WLog_DBG(TAG, "Exponent:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->Exponent, 4);
#endif

	EncryptedPremasterSecret = (BYTE*)calloc(1, license->ModulusLength);
	if (!EncryptedPremasterSecret)
		return FALSE;

	license->EncryptedPremasterSecret->type = BB_RANDOM_BLOB;
	license->EncryptedPremasterSecret->length = PREMASTER_SECRET_LENGTH;
#ifndef LICENSE_NULL_PREMASTER_SECRET
	license->EncryptedPremasterSecret->length = crypto_rsa_public_encrypt(
	    license->PremasterSecret, PREMASTER_SECRET_LENGTH, license->ModulusLength, license->Modulus,
	    license->Exponent, EncryptedPremasterSecret);
#endif
	license->EncryptedPremasterSecret->data = EncryptedPremasterSecret;
	return TRUE;
}