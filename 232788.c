_gnutls_x509_der_encode_and_copy(ASN1_TYPE src, const char *src_name,
				 ASN1_TYPE dest, const char *dest_name,
				 int str)
{
	int result;
	gnutls_datum_t encoded;

	result = _gnutls_x509_der_encode(src, src_name, &encoded, str);

	if (result < 0) {
		gnutls_assert();
		return result;
	}

	/* Write the data.
	 */
	result =
	    asn1_write_value(dest, dest_name, encoded.data,
			     (int) encoded.size);

	_gnutls_free_datum(&encoded);

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	return 0;
}