BOOL x509_verify_certificate(CryptoCert cert, const char* certificate_store_path)
{
	size_t i;
	const int purposes[3] = { X509_PURPOSE_SSL_SERVER, X509_PURPOSE_SSL_CLIENT, X509_PURPOSE_ANY };
	X509_STORE_CTX* csc;
	BOOL status = FALSE;
	X509_STORE* cert_ctx = NULL;
	X509_LOOKUP* lookup = NULL;
	cert_ctx = X509_STORE_new();

	if (cert_ctx == NULL)
		goto end;

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
	OpenSSL_add_all_algorithms();
#else
	OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS |
	                        OPENSSL_INIT_LOAD_CONFIG,
	                    NULL);
#endif

	lookup = X509_STORE_add_lookup(cert_ctx, X509_LOOKUP_file());

	if (lookup == NULL)
		goto end;

	lookup = X509_STORE_add_lookup(cert_ctx, X509_LOOKUP_hash_dir());

	if (lookup == NULL)
		goto end;

	X509_LOOKUP_add_dir(lookup, NULL, X509_FILETYPE_DEFAULT);

	if (certificate_store_path != NULL)
	{
		X509_LOOKUP_add_dir(lookup, certificate_store_path, X509_FILETYPE_PEM);
	}

	X509_STORE_set_flags(cert_ctx, 0);

	for (i = 0; i < ARRAYSIZE(purposes); i++)
	{
		int rc = -1;
		int purpose = purposes[i];
		csc = X509_STORE_CTX_new();

		if (csc == NULL)
			goto skip;
		if (!X509_STORE_CTX_init(csc, cert_ctx, cert->px509, cert->px509chain))
			goto skip;

		X509_STORE_CTX_set_purpose(csc, purpose);
		X509_STORE_CTX_set_verify_cb(csc, verify_cb);

		rc = X509_verify_cert(csc);
	skip:
		X509_STORE_CTX_free(csc);
		if (rc == 1)
		{
			status = TRUE;
			break;
		}
	}

	X509_STORE_free(cert_ctx);
end:
	return status;
}