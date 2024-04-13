_gnutls_x509_read_string(ASN1_TYPE c, const char *root,
			 gnutls_datum_t * ret, unsigned int etype, unsigned int allow_ber)
{
	int len = 0, result;
	size_t slen;
	uint8_t *tmp = NULL;
	unsigned rtype;

	result = asn1_read_value_type(c, root, NULL, &len, &rtype);
	if (result != ASN1_MEM_ERROR) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		return result;
	}

	if (rtype == ASN1_ETYPE_BIT_STRING)
		len /= 8;

	tmp = gnutls_malloc((size_t) len + 1);
	if (tmp == NULL) {
		gnutls_assert();
		result = GNUTLS_E_MEMORY_ERROR;
		goto cleanup;
	}

	result = asn1_read_value(c, root, tmp, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (rtype == ASN1_ETYPE_BIT_STRING)
		len /= 8;

	/* Extract the STRING.
	 */
	slen = (size_t) len;

	result = _gnutls_x509_decode_string(etype, tmp, slen, ret, allow_ber);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}
	gnutls_free(tmp);

	return 0;

      cleanup:
	gnutls_free(tmp);
	return result;
}