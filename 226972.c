static BOOL license_generate_keys(rdpLicense* license)
{
	BOOL ret;

	if (
	    /* MasterSecret */
	    !security_master_secret(license->PremasterSecret, license->ClientRandom,
	                            license->ServerRandom, license->MasterSecret) ||
	    /* SessionKeyBlob */
	    !security_session_key_blob(license->MasterSecret, license->ClientRandom,
	                               license->ServerRandom, license->SessionKeyBlob))
	{
		return FALSE;
	}
	security_mac_salt_key(license->SessionKeyBlob, license->ClientRandom, license->ServerRandom,
	                      license->MacSaltKey); /* MacSaltKey */
	ret = security_licensing_encryption_key(
	    license->SessionKeyBlob, license->ClientRandom, license->ServerRandom,
	    license->LicensingEncryptionKey); /* LicensingEncryptionKey */
#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "ClientRandom:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->ClientRandom, CLIENT_RANDOM_LENGTH);
	WLog_DBG(TAG, "ServerRandom:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->ServerRandom, SERVER_RANDOM_LENGTH);
	WLog_DBG(TAG, "PremasterSecret:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->PremasterSecret, PREMASTER_SECRET_LENGTH);
	WLog_DBG(TAG, "MasterSecret:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->MasterSecret, MASTER_SECRET_LENGTH);
	WLog_DBG(TAG, "SessionKeyBlob:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->SessionKeyBlob, SESSION_KEY_BLOB_LENGTH);
	WLog_DBG(TAG, "MacSaltKey:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->MacSaltKey, MAC_SALT_KEY_LENGTH);
	WLog_DBG(TAG, "LicensingEncryptionKey:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->LicensingEncryptionKey,
	              LICENSING_ENCRYPTION_KEY_LENGTH);
#endif
	return ret;
}