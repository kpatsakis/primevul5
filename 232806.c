_gnutls_x509_get_signature(ASN1_TYPE src, const char *src_name,
			   gnutls_datum_t * signature)
{
	int result, len;
	int bits;

	signature->data = NULL;
	signature->size = 0;

	/* Read the signature 
	 */
	len = 0;
	result = asn1_read_value(src, src_name, NULL, &len);

	if (result != ASN1_MEM_ERROR) {
		result = _gnutls_asn2err(result);
		gnutls_assert();
		goto cleanup;
	}

	bits = len;
	if (bits % 8 != 0 || bits < 8) {
		gnutls_assert();
		result = GNUTLS_E_CERTIFICATE_ERROR;
		goto cleanup;
	}

	len = bits / 8;

	signature->data = gnutls_malloc(len);
	if (signature->data == NULL) {
		gnutls_assert();
		result = GNUTLS_E_MEMORY_ERROR;
		return result;
	}

	/* read the bit string of the signature
	 */
	bits = len;
	result =
	    asn1_read_value(src, src_name, signature->data, &bits);

	if (result != ASN1_SUCCESS) {
		result = _gnutls_asn2err(result);
		gnutls_assert();
		goto cleanup;
	}

	signature->size = len;

	return 0;

      cleanup:
	return result;
}