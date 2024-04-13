_gnutls_x509_write_string(ASN1_TYPE c, const char *root,
			  const gnutls_datum_t * data, unsigned int etype)
{
	int ret;
	gnutls_datum_t val = { NULL, 0 };

	ret =
	    _gnutls_x509_encode_string(etype, data->data, data->size,
				       &val);
	if (ret < 0)
		return gnutls_assert_val(ret);

	/* Write the data.
	 */
	ret = asn1_write_value(c, root, val.data, val.size);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		goto cleanup;
	}

	ret = 0;

      cleanup:
	_gnutls_free_datum(&val);
	return ret;
}