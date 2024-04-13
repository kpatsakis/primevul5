p11_rpc_buffer_get_rsa_pkcs_pss_mechanism_value (p11_buffer *buffer,
						 size_t *offset,
						 void *value,
						 CK_ULONG *value_length)
{
	uint64_t val[3];

	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val[0]))
		return false;
	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val[1]))
		return false;
	if (!p11_rpc_buffer_get_uint64 (buffer, offset, &val[2]))
		return false;

	if (value) {
		CK_RSA_PKCS_PSS_PARAMS params;

		params.hashAlg = val[0];
		params.mgf = val[1];
		params.sLen = val[2];

		memcpy (value, &params, sizeof (CK_RSA_PKCS_PSS_PARAMS));
	}

	if (value_length)
		*value_length = sizeof (CK_RSA_PKCS_PSS_PARAMS);

	return true;
}