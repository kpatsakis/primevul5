_gnutls_x509_get_signed_data(ASN1_TYPE src,  const gnutls_datum *_der,
			     const char *src_name,
			     gnutls_datum_t * signed_data)
{
	int start, end, result;
	gnutls_datum_t der;
	unsigned need_free = 0;

	if (_der == NULL || _der->size == 0) {
		result = _gnutls_x509_der_encode(src, "", &der, 0);
		if (result < 0) {
			gnutls_assert();
			return result;
		}
		need_free = 1;
	} else {
		der.data = _der->data;
		der.size = _der->size;
	}

	/* Get the signed data
	 */
	result = asn1_der_decoding_startEnd(src, der.data, der.size,
					    src_name, &start, &end);
	if (result != ASN1_SUCCESS) {
		result = _gnutls_asn2err(result);
		gnutls_assert();
		goto cleanup;
	}

	result =
	    _gnutls_set_datum(signed_data, &der.data[start],
			      end - start + 1);

	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = 0;

      cleanup:
	if (need_free != 0)
		_gnutls_free_datum(&der);

	return result;
}