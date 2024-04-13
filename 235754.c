static bool validate_cert(char *signing_cert, int signing_cert_size)
{
	mbedtls_x509_crt x509;
	char *x509_buf = NULL;
	int rc;

	mbedtls_x509_crt_init(&x509);
	rc = mbedtls_x509_crt_parse(&x509, signing_cert, signing_cert_size);

	/* If failure in parsing the certificate, exit */
	if(rc) {
		prlog(PR_ERR, "X509 certificate parsing failed %04x\n", rc);
		return false;
	}

	x509_buf = zalloc(CERT_BUFFER_SIZE);
	rc = mbedtls_x509_crt_info(x509_buf, CERT_BUFFER_SIZE, "CRT:", &x509);

	mbedtls_x509_crt_free(&x509);
	free(x509_buf);
	x509_buf = NULL;

	/* If failure in reading the certificate, exit */
	if (rc < 0)
		return false;

	return true;
}