BOOL license_read_platform_challenge_packet(rdpLicense* license, wStream* s)
{
	BYTE macData[16];
	UINT32 ConnectFlags = 0;

	DEBUG_LICENSE("Receiving Platform Challenge Packet");

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT32(s, ConnectFlags); /* ConnectFlags, Reserved (4 bytes) */

	/* EncryptedPlatformChallenge */
	license->EncryptedPlatformChallenge->type = BB_ANY_BLOB;
	if (!license_read_binary_blob(s, license->EncryptedPlatformChallenge))
		return FALSE;
	license->EncryptedPlatformChallenge->type = BB_ENCRYPTED_DATA_BLOB;

	/* MACData (16 bytes) */
	if (Stream_GetRemainingLength(s) < 16)
		return FALSE;

	Stream_Read(s, macData, 16);
	if (!license_decrypt_and_check_MAC(license, license->EncryptedPlatformChallenge->data,
	                                   license->EncryptedPlatformChallenge->length,
	                                   license->PlatformChallenge, macData))
		return FALSE;

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "ConnectFlags: 0x%08" PRIX32 "", ConnectFlags);
	WLog_DBG(TAG, "EncryptedPlatformChallenge:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->EncryptedPlatformChallenge->data,
	              license->EncryptedPlatformChallenge->length);
	WLog_DBG(TAG, "PlatformChallenge:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->PlatformChallenge->data,
	              license->PlatformChallenge->length);
	WLog_DBG(TAG, "MacData:");
	winpr_HexDump(TAG, WLOG_DEBUG, macData, 16);
#endif
	return TRUE;
}