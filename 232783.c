_gnutls_x509_write_value(ASN1_TYPE c, const char *root,
			 const gnutls_datum_t * data)
{
	int ret;

	/* Write the data.
	 */
	ret = asn1_write_value(c, root, data->data, data->size);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	return 0;
}