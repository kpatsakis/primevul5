_gnutls_x509_get_raw_field2(ASN1_TYPE c2, gnutls_datum_t * raw,
			 const char *whom, gnutls_datum_t * dn)
{
	int result, len1;
	int start1, end1;
	result =
	    asn1_der_decoding_startEnd(c2, raw->data, raw->size,
				       whom, &start1, &end1);

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	len1 = end1 - start1 + 1;

	dn->data = &raw->data[start1];
	dn->size = len1;
	result = 0;

      cleanup:
	return result;
}