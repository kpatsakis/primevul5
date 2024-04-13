_gnutls_x509_der_encode(ASN1_TYPE src, const char *src_name,
			gnutls_datum_t * res, int str)
{
	int size, result;
	int asize;
	uint8_t *data = NULL;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	size = 0;
	result = asn1_der_coding(src, src_name, NULL, &size, NULL);
	/* this check explicitly covers the case where size == 0 && result == 0 */
	if (result != ASN1_MEM_ERROR) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	/* allocate data for the der
	 */

	if (str)
		size += 16;	/* for later to include the octet tags */
	asize = size;

	data = gnutls_malloc((size_t) size);
	if (data == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	result = asn1_der_coding(src, src_name, data, &size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (str) {
		if ((result = asn1_create_element
		     (_gnutls_get_pkix(), "PKIX1.pkcs-7-Data",
		      &c2)) != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}

		result = asn1_write_value(c2, "", data, size);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}

		result = asn1_der_coding(c2, "", data, &asize, NULL);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}

		size = asize;

		asn1_delete_structure(&c2);
	}

	res->data = data;
	res->size = (unsigned) size;
	return 0;

      cleanup:
	gnutls_free(data);
	asn1_delete_structure(&c2);
	return result;

}