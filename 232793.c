int _gnutls_x509_encode_string(unsigned int etype,
			       const void *input_data, size_t input_size,
			       gnutls_datum_t * output)
{
	uint8_t tl[ASN1_MAX_TL_SIZE];
	unsigned int tl_size;
	int ret;

	tl_size = sizeof(tl);
	ret =
	    asn1_encode_simple_der(etype, input_data, input_size, tl,
				   &tl_size);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		return ret;
	}

	output->data = gnutls_malloc(tl_size + input_size);
	if (output->data == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	memcpy(output->data, tl, tl_size);
	memcpy(output->data + tl_size, input_data, input_size);

	output->size = tl_size + input_size;

	return 0;
}