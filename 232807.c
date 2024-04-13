_gnutls_x509_get_pk_algorithm(ASN1_TYPE src, const char *src_name,
			      unsigned int *bits)
{
	int result;
	int algo;
	char oid[64];
	int len;
	gnutls_pk_params_st params;
	char name[128];

	gnutls_pk_params_init(&params);

	_asnstr_append_name(name, sizeof(name), src_name,
			    ".algorithm.algorithm");
	len = sizeof(oid);
	result = asn1_read_value(src, name, oid, &len);

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	algo = _gnutls_x509_oid2pk_algorithm(oid);
	if (algo == GNUTLS_PK_UNKNOWN) {
		_gnutls_debug_log
		    ("%s: unknown public key algorithm: %s\n", __func__,
		     oid);
	}

	if (bits == NULL) {
		return algo;
	}

	/* Now read the parameters' bits 
	 */
	result = _gnutls_get_asn_mpis(src, src_name, &params);
	if (result < 0)
		return gnutls_assert_val(result);

	bits[0] = pubkey_to_bits(algo, &params);

	gnutls_pk_params_release(&params);
	return algo;
}