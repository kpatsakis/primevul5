_gnutls_x509_export_int_named2(ASN1_TYPE asn1_data, const char *name,
			       gnutls_x509_crt_fmt_t format,
			       const char *pem_header,
			       gnutls_datum_t * out)
{
	int ret;

	if (format == GNUTLS_X509_FMT_DER) {
		ret = _gnutls_x509_der_encode(asn1_data, name, out, 0);
		if (ret < 0)
			return gnutls_assert_val(ret);
	} else {		/* PEM */
		gnutls_datum_t tmp;

		ret = _gnutls_x509_der_encode(asn1_data, name, &tmp, 0);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret =
		    _gnutls_fbase64_encode(pem_header, tmp.data, tmp.size,
					   out);
		_gnutls_free_datum(&tmp);

		if (ret < 0)
			return gnutls_assert_val(ret);
	}

	return 0;
}