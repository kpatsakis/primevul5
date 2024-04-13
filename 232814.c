int x509_crt_to_raw_pubkey(gnutls_x509_crt_t crt,
			   gnutls_datum_t * rpubkey)
{
	gnutls_pubkey_t pubkey = NULL;
	int ret;

	ret = gnutls_pubkey_init(&pubkey);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = gnutls_pubkey_import_x509(pubkey, crt, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret =
	    gnutls_pubkey_export2(pubkey, GNUTLS_X509_FMT_DER, rpubkey);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

      cleanup:
	gnutls_pubkey_deinit(pubkey);
	return ret;
}