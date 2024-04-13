BOOL license_generate_hwid(rdpLicense* license)
{
	const BYTE* hashTarget;
	size_t targetLen;
	BYTE macAddress[6];

	ZeroMemory(license->HardwareId, HWID_LENGTH);

	if (license->rdp->settings->OldLicenseBehaviour)
	{
		ZeroMemory(macAddress, sizeof(macAddress));
		hashTarget = macAddress;
		targetLen = sizeof(macAddress);
	}
	else
	{
		wStream s;
		const char* hostname = license->rdp->settings->ClientHostname;
		Stream_StaticInit(&s, license->HardwareId, 4);
		Stream_Write_UINT32(&s, PLATFORMID);
		Stream_Free(&s, TRUE);

		hashTarget = (const BYTE*)hostname;
		targetLen = strlen(hostname);
	}

	/* Allow FIPS override for use of MD5 here, really this does not have to be MD5 as we are just
	 * taking a MD5 hash of the 6 bytes of 0's(macAddress) */
	/* and filling in the Data1-Data4 fields of the CLIENT_HARDWARE_ID structure(from MS-RDPELE
	 * section 2.2.2.3.1). This is for RDP licensing packets */
	/* which will already be encrypted under FIPS, so the use of MD5 here is not for sensitive data
	 * protection. */
	return winpr_Digest_Allow_FIPS(WINPR_MD_MD5, hashTarget, targetLen,
	                               &license->HardwareId[HWID_PLATFORM_ID_LENGTH],
	                               WINPR_MD5_DIGEST_LENGTH);
}