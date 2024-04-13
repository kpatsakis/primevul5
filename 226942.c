static BOOL license_encrypt_and_MAC(rdpLicense* license, const BYTE* input, size_t len,
                                    LICENSE_BLOB* target, BYTE* mac)
{
	return license_rc4_with_licenseKey(license, input, len, target) &&
	       security_mac_data(license->MacSaltKey, input, len, mac);
}