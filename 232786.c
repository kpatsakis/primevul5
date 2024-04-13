_gnutls_x509_decode_string(unsigned int etype,
			   const uint8_t * der, size_t der_size,
			   gnutls_datum_t * output, unsigned allow_ber)
{
	int ret;
	uint8_t *str;
	unsigned int str_size, len;
	gnutls_datum_t td;

#ifdef HAVE_ASN1_DECODE_SIMPLE_BER
	if (allow_ber)
		ret =
		    asn1_decode_simple_ber(etype, der, der_size, &str, &str_size, NULL);
	else
#endif
		ret =
		    asn1_decode_simple_der(etype, der, der_size, (const uint8_t**)&str, &str_size);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		return ret;
	}

	td.size = str_size;
	td.data = gnutls_malloc(str_size + 1);
	if (td.data == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	memcpy(td.data, str, str_size);
	td.data[str_size] = 0;

#ifdef HAVE_ASN1_DECODE_SIMPLE_BER
	if (allow_ber)
		free(str);
#endif

	ret = make_printable_string(etype, &td, output);
	if (ret == GNUTLS_E_INVALID_REQUEST) {	/* unsupported etype */
		output->data = td.data;
		output->size = td.size;
		ret = 0;
	} else if (ret <= 0) {
		_gnutls_free_datum(&td);
	}

	/* Refuse to deal with strings containing NULs. */
	if (etype != ASN1_ETYPE_OCTET_STRING) {
		if (output->data)
			len = strlen((void *) output->data);
		else
			len = 0;

		if (len != (size_t) output->size) {
			_gnutls_free_datum(output);
			ret = gnutls_assert_val(GNUTLS_E_ASN1_DER_ERROR);
		}
	}

	return ret;
}