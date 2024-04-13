p11_rpc_buffer_add_rsa_pkcs_oaep_mechanism_value (p11_buffer *buffer,
						  const void *value,
						  CK_ULONG value_length)
{
	CK_RSA_PKCS_OAEP_PARAMS params;

	/* Check if value can be converted to CK_RSA_PKCS_OAEP_PARAMS. */
	if (value_length != sizeof (CK_RSA_PKCS_OAEP_PARAMS)) {
		p11_buffer_fail (buffer);
		return;
	}

	memcpy (&params, value, value_length);

	/* Check if params.hashAlg, params.mgf, and params.source can be
	 * converted to uint64_t. */
	if (params.hashAlg > UINT64_MAX || params.mgf > UINT64_MAX ||
	    params.source > UINT64_MAX) {
		p11_buffer_fail (buffer);
		return;
	}

	p11_rpc_buffer_add_uint64 (buffer, params.hashAlg);
	p11_rpc_buffer_add_uint64 (buffer, params.mgf);
	p11_rpc_buffer_add_uint64 (buffer, params.source);

	/* parmas.pSourceData can only be an array of CK_BYTE or
	 * NULL */
	p11_rpc_buffer_add_byte_array (buffer,
				       (unsigned char *)params.pSourceData,
				       params.ulSourceDataLen);
}