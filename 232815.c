_gnutls_x509_get_signature_algorithm(ASN1_TYPE src, const char *src_name)
{
	int result;
	gnutls_datum_t sa;

	/* Read the signature algorithm. Note that parameters are not
	 * read. They will be read from the issuer's certificate if needed.
	 */
	result = _gnutls_x509_read_value(src, src_name, &sa);

	if (result < 0) {
		gnutls_assert();
		return result;
	}

	result = _gnutls_x509_oid2sign_algorithm((char *) sa.data);

	_gnutls_free_datum(&sa);

	return result;
}