static size_t get_pkcs7_len(const struct efi_variable_authentication_2 *auth)
{
	uint32_t dw_length;
	size_t size;

	assert(auth != NULL);

	dw_length = le32_to_cpu(auth->auth_info.hdr.dw_length);
	size = dw_length - (sizeof(auth->auth_info.hdr.dw_length)
			+ sizeof(auth->auth_info.hdr.w_revision)
			+ sizeof(auth->auth_info.hdr.w_certificate_type)
			+ sizeof(auth->auth_info.cert_type));

	return size;
}