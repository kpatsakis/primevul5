static mbedtls_pkcs7* get_pkcs7(const struct efi_variable_authentication_2 *auth)
{
	char *checkpkcs7cert = NULL;
	size_t len;
	mbedtls_pkcs7 *pkcs7 = NULL;
	int rc;

	len = get_pkcs7_len(auth);

	pkcs7 = malloc(sizeof(struct mbedtls_pkcs7));
	if (!pkcs7)
		return NULL;

	mbedtls_pkcs7_init(pkcs7);
	rc = mbedtls_pkcs7_parse_der( auth->auth_info.cert_data, len, pkcs7);
	if (rc <= 0) {
		prlog(PR_ERR, "Parsing pkcs7 failed %04x\n", rc);
		goto out;
	}

	checkpkcs7cert = zalloc(CERT_BUFFER_SIZE);
	if (!checkpkcs7cert)
		goto out;

	rc = mbedtls_x509_crt_info(checkpkcs7cert, CERT_BUFFER_SIZE, "CRT:",
				   &(pkcs7->signed_data.certs));
	if (rc < 0) {
		prlog(PR_ERR, "Failed to parse the certificate in PKCS7 structure\n");
		free(checkpkcs7cert);
		goto out;
	}

	prlog(PR_DEBUG, "%s \n", checkpkcs7cert);
	free(checkpkcs7cert);
	return pkcs7;

out:
	mbedtls_pkcs7_free(pkcs7);
	pkcs7 = NULL;
	return pkcs7;
}