static BOOL license_decrypt_and_check_MAC(rdpLicense* license, const BYTE* input, size_t len,
                                          LICENSE_BLOB* target, const BYTE* packetMac)
{
	BYTE macData[16];

	return license_rc4_with_licenseKey(license, input, len, target) &&
	       security_mac_data(license->MacSaltKey, target->data, len, macData) &&
	       (memcmp(packetMac, macData, sizeof(macData)) == 0);
}