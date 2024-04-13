BOOL license_read_license_request_packet(rdpLicense* license, wStream* s)
{
	/* ServerRandom (32 bytes) */
	if (Stream_GetRemainingLength(s) < 32)
		return FALSE;

	Stream_Read(s, license->ServerRandom, 32);

	/* ProductInfo */
	if (!license_read_product_info(s, license->ProductInfo))
		return FALSE;

	/* KeyExchangeList */
	if (!license_read_binary_blob(s, license->KeyExchangeList))
		return FALSE;

	/* ServerCertificate */
	if (!license_read_binary_blob(s, license->ServerCertificate))
		return FALSE;

	/* ScopeList */
	if (!license_read_scope_list(s, license->ScopeList))
		return FALSE;

	/* Parse Server Certificate */
	if (!certificate_read_server_certificate(license->certificate, license->ServerCertificate->data,
	                                         license->ServerCertificate->length))
		return FALSE;

	if (!license_generate_keys(license) || !license_generate_hwid(license) ||
	    !license_encrypt_premaster_secret(license))
		return FALSE;

#ifdef WITH_DEBUG_LICENSE
	WLog_DBG(TAG, "ServerRandom:");
	winpr_HexDump(TAG, WLOG_DEBUG, license->ServerRandom, 32);
	license_print_product_info(license->ProductInfo);
	license_print_scope_list(license->ScopeList);
#endif
	return TRUE;
}