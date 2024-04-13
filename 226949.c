BOOL license_write_new_license_request_packet(rdpLicense* license, wStream* s)
{
	UINT32 PlatformId = PLATFORMID;
	UINT32 PreferredKeyExchangeAlg = KEY_EXCHANGE_ALG_RSA;

	Stream_Write_UINT32(s, PreferredKeyExchangeAlg); /* PreferredKeyExchangeAlg (4 bytes) */
	Stream_Write_UINT32(s, PlatformId);              /* PlatformId (4 bytes) */
	Stream_Write(s, license->ClientRandom, 32);      /* ClientRandom (32 bytes) */

	if (/* EncryptedPremasterSecret */
	    !license_write_encrypted_premaster_secret_blob(s, license->EncryptedPremasterSecret,
	                                                   license->ModulusLength) ||
	    /* ClientUserName */
	    !license_write_binary_blob(s, license->ClientUserName) ||
	    /* ClientMachineName */
	    !license_write_binary_blob(s, license->ClientMachineName))
	{
		return FALSE;
	}

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "PreferredKeyExchangeAlg: 0x%08" PRIX32 "", PreferredKeyExchangeAlg);
	WLog_DBG(TAG, "ClientRandom:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->ClientRandom, 32);
	WLog_DBG(TAG, "EncryptedPremasterSecret");
	winpr_HexDump(TAG, WLOG_DEBUG, license->EncryptedPremasterSecret->data,
	              license->EncryptedPremasterSecret->length);
	WLog_DBG(TAG, "ClientUserName (%" PRIu16 "): %s", license->ClientUserName->length,
	         (char*)license->ClientUserName->data);
	WLog_DBG(TAG, "ClientMachineName (%" PRIu16 "): %s", license->ClientMachineName->length,
	         (char*)license->ClientMachineName->data);
#endif
	return TRUE;
}