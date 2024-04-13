int get_auth_descriptor2(const void *buf, const size_t buflen, void **auth_buffer)
{
	const struct efi_variable_authentication_2 *auth = buf;
	int auth_buffer_size;
	size_t len;

	assert(auth_buffer != NULL);
	if (buflen < sizeof(struct efi_variable_authentication_2)
	    || !buf)
			return OPAL_PARAMETER;

	len = get_pkcs7_len(auth);
	/* pkcs7 content length cannot be greater than buflen */ 
	if (len > buflen)
		return OPAL_PARAMETER;

	auth_buffer_size = sizeof(auth->timestamp) + sizeof(auth->auth_info.hdr)
			   + sizeof(auth->auth_info.cert_type) + len;

	*auth_buffer = zalloc(auth_buffer_size);
	if (!(*auth_buffer))
		return OPAL_NO_MEM;

	/*
	 * Data = auth descriptor + new ESL data.
	 * Extracts only the auth descriptor from data.
	 */
	memcpy(*auth_buffer, buf, auth_buffer_size);

	return auth_buffer_size;
}