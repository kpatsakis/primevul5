_gnutls_x509_read_value(ASN1_TYPE c, const char *root,
			gnutls_datum_t * ret)
{
	int len = 0, result;
	uint8_t *tmp = NULL;
	unsigned int etype;

	result = asn1_read_value_type(c, root, NULL, &len, &etype);
	if (result == 0 && len == 0) {
		/* don't allow null strings */
		return gnutls_assert_val(GNUTLS_E_ASN1_DER_ERROR);
	}

	if (result != ASN1_MEM_ERROR) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		return result;
	}

	if (etype == ASN1_ETYPE_BIT_STRING) {
		len = (len + 7)/8;
	}

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

	if (etype == ASN1_ETYPE_BIT_STRING) {
		ret->size = (len+7) / 8;
	} else {
		ret->size = (unsigned) len;
	}

	tmp[ret->size] = 0;
	ret->data = tmp;

	return 0;

      cleanup:
	gnutls_free(tmp);
	return result;
}