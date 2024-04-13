bool is_pkcs7_sig_format(const void *data)
{
	const struct efi_variable_authentication_2 *auth = data;
	uuid_t pkcs7_guid = EFI_CERT_TYPE_PKCS7_GUID;

	return !memcmp(&auth->auth_info.cert_type, &pkcs7_guid, 16);
}