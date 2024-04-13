BOOL license_send_platform_challenge_response_packet(rdpLicense* license)
{
	wStream* s;
	wStream* challengeRespData;
	int length;
	BYTE* buffer;
	BYTE mac_data[16];
	BOOL status;

	DEBUG_LICENSE("Sending Platform Challenge Response Packet");
	s = license_send_stream_init(license);
	license->EncryptedPlatformChallenge->type = BB_DATA_BLOB;

	/* prepare the PLATFORM_CHALLENGE_RESPONSE_DATA */
	challengeRespData = Stream_New(NULL, 8 + license->PlatformChallenge->length);
	if (!challengeRespData)
		return FALSE;
	Stream_Write_UINT16(challengeRespData, 0x0100);                        /* wVersion */
	Stream_Write_UINT16(challengeRespData, OTHER_PLATFORM_CHALLENGE_TYPE); /* wClientType */
	Stream_Write_UINT16(challengeRespData, LICENSE_DETAIL_DETAIL);         /* wLicenseDetailLevel */
	Stream_Write_UINT16(challengeRespData, license->PlatformChallenge->length); /* cbChallenge */
	Stream_Write(challengeRespData, license->PlatformChallenge->data,
	             license->PlatformChallenge->length); /* pbChallenge */
	Stream_SealLength(challengeRespData);

	/* compute MAC of PLATFORM_CHALLENGE_RESPONSE_DATA + HWID */
	length = Stream_Length(challengeRespData) + HWID_LENGTH;
	buffer = (BYTE*)malloc(length);
	if (!buffer)
	{
		Stream_Free(challengeRespData, TRUE);
		return FALSE;
	}

	CopyMemory(buffer, Stream_Buffer(challengeRespData), Stream_Length(challengeRespData));
	CopyMemory(&buffer[Stream_Length(challengeRespData)], license->HardwareId, HWID_LENGTH);
	status = security_mac_data(license->MacSaltKey, buffer, length, mac_data);
	free(buffer);

	if (!status)
	{
		Stream_Free(challengeRespData, TRUE);
		return FALSE;
	}

	license->EncryptedHardwareId->type = BB_ENCRYPTED_DATA_BLOB;
	if (!license_rc4_with_licenseKey(license, license->HardwareId, HWID_LENGTH,
	                                 license->EncryptedHardwareId))
	{
		Stream_Free(challengeRespData, TRUE);
		return FALSE;
	}

	status = license_rc4_with_licenseKey(license, Stream_Buffer(challengeRespData),
	                                     Stream_Length(challengeRespData),
	                                     license->EncryptedPlatformChallengeResponse);
	Stream_Free(challengeRespData, TRUE);
	if (!status)
		return FALSE;

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "LicensingEncryptionKey:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->LicensingEncryptionKey, 16);
	WLog_DBG(TAG, "HardwareId:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->HardwareId, HWID_LENGTH);
	WLog_DBG(TAG, "EncryptedHardwareId:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->EncryptedHardwareId->data, HWID_LENGTH);
#endif
	if (license_write_platform_challenge_response_packet(license, s, mac_data))
		return license_send(license, s, PLATFORM_CHALLENGE_RESPONSE);

	Stream_Release(s);
	return FALSE;
}