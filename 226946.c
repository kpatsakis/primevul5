BOOL license_write_platform_challenge_response_packet(rdpLicense* license, wStream* s,
                                                      const BYTE* macData)
{
	if (!license_write_binary_blob(
	        s,
	        license->EncryptedPlatformChallengeResponse) || /* EncryptedPlatformChallengeResponse */
	    !license_write_binary_blob(s, license->EncryptedHardwareId) || /* EncryptedHWID */
	    !Stream_EnsureRemainingCapacity(s, 16))
	{
		return FALSE;
	}

	Stream_Write(s, macData, 16); /* MACData */
	return TRUE;
}