p11_rpc_buffer_get_rsa_pkcs_oaep_mechanism_value (p11_buffer *buffer,
						  size_t *offset,
						  void *value,
						  CK_ULONG *value_length)
{
	uint64_t val[3];
	const unsigned char *data;
	size_t len;

	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val[0]))
		return false;
	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val[1]))
		return false;
	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val[2]))
		return false;
	if (!p11_rpc_buffer_get_byte_array (buffer, offset, &data, &len))
		return false;

	if (value) {
		CK_RSA_PKCS_OAEP_PARAMS params;

		params.hashAlg = val[0];
		params.mgf = val[1];
		params.source = val[2];
		params.pSourceData = (void *) data;
		params.ulSourceDataLen = len;

		memcpy (value, &params, sizeof (CK_RSA_PKCS_OAEP_PARAMS));
	}

	if (value_length)
		*value_length = sizeof (CK_RSA_PKCS_OAEP_PARAMS);

	return true;
}