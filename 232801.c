int x509_raw_crt_to_raw_pubkey(const gnutls_datum_t * cert,
			   gnutls_datum_t * rpubkey)
{
	gnutls_x509_crt_t crt = NULL;
	int ret;

	ret = gnutls_x509_crt_init(&crt);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = gnutls_x509_crt_import(crt, cert, GNUTLS_X509_FMT_DER);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = x509_crt_to_raw_pubkey(crt, rpubkey);
 cleanup:
	gnutls_x509_crt_deinit(crt);

	return ret;
}