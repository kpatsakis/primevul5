static int verify_signature(const struct efi_variable_authentication_2 *auth,
			    const char *newcert, const size_t new_data_size,
			    const struct secvar *avar)
{
	mbedtls_pkcs7 *pkcs7 = NULL;
	mbedtls_x509_crt x509;
	char *signing_cert = NULL;
	char *x509_buf = NULL;
	int signing_cert_size;
	int rc = 0;
	char *errbuf;
	int eslvarsize;
	int eslsize;
	int offset = 0;

	if (!auth)
		return OPAL_PARAMETER;

	/* Extract the pkcs7 from the auth structure */
	pkcs7 = get_pkcs7(auth);
	/* Failure to parse pkcs7 implies bad input. */
	if (!pkcs7)
		return OPAL_PARAMETER;

	prlog(PR_INFO, "Load the signing certificate from the keystore");

	eslvarsize = avar->data_size;

	/* Variable is not empty */
	while (eslvarsize > 0) {
		prlog(PR_DEBUG, "esl var size size is %d offset is %d\n", eslvarsize, offset);
		if (eslvarsize < sizeof(EFI_SIGNATURE_LIST))
			break;

		/* Calculate the size of the ESL */
		eslsize = get_esl_signature_list_size(avar->data + offset,
						      eslvarsize);
		/* If could not extract the size */
		if (eslsize <= 0) {
			rc = OPAL_PARAMETER;
			break;
		}

		/* Extract the certificate from the ESL */
		signing_cert_size = get_esl_cert(avar->data + offset,
						 eslvarsize, &signing_cert);
		if (signing_cert_size < 0) {
			rc = signing_cert_size;
			break;
		}

		mbedtls_x509_crt_init(&x509);
		rc = mbedtls_x509_crt_parse(&x509,
					    signing_cert,
					    signing_cert_size);

		/* This should not happen, unless something corrupted in PNOR */
		if(rc) {
			prlog(PR_ERR, "X509 certificate parsing failed %04x\n", rc);
			rc = OPAL_INTERNAL_ERROR;
			break;
		}

		x509_buf = zalloc(CERT_BUFFER_SIZE);
		rc = mbedtls_x509_crt_info(x509_buf,
					   CERT_BUFFER_SIZE,
					   "CRT:",
					   &x509);

		/* This should not happen, unless something corrupted in PNOR */
		if (rc < 0) {
			free(x509_buf);
			rc = OPAL_INTERNAL_ERROR;
			break;
		}

		prlog(PR_INFO, "%s \n", x509_buf);
		free(x509_buf);
		x509_buf = NULL;

		rc = mbedtls_pkcs7_signed_hash_verify(pkcs7, &x509, newcert, new_data_size);

		/* If you find a signing certificate, you are done */
		if (rc == 0) {
			prlog(PR_INFO, "Signature Verification passed\n");
			mbedtls_x509_crt_free(&x509);
			break;
		} else {
			errbuf = zalloc(MBEDTLS_ERR_BUFFER_SIZE);
			mbedtls_strerror(rc, errbuf, MBEDTLS_ERR_BUFFER_SIZE);
			prlog(PR_ERR, "Signature Verification failed %02x %s\n",
					rc, errbuf);
			free(errbuf);
			rc = OPAL_PERMISSION;
		}


		/* Look for the next ESL */
		offset = offset + eslsize;
		eslvarsize = eslvarsize - eslsize;
		mbedtls_x509_crt_free(&x509);
		free(signing_cert);
		/* Since we are going to allocate again in the next iteration */
		signing_cert = NULL;

	}

	free(signing_cert);
	mbedtls_pkcs7_free(pkcs7);
	free(pkcs7);

	return rc;
}