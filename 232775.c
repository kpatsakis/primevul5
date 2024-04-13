_gnutls_x509_encode_and_copy_PKI_params(ASN1_TYPE dst,
					const char *dst_name,
					gnutls_pk_algorithm_t
					pk_algorithm,
					gnutls_pk_params_st * params)
{
	const char *pk;
	gnutls_datum_t der = { NULL, 0 };
	int result;
	char name[128];

	pk = _gnutls_x509_pk_to_oid(pk_algorithm);
	if (pk == NULL) {
		gnutls_assert();
		return GNUTLS_E_UNKNOWN_PK_ALGORITHM;
	}

	/* write the OID
	 */
	_asnstr_append_name(name, sizeof(name), dst_name,
			    ".algorithm.algorithm");

	result = asn1_write_value(dst, name, pk, 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result =
	    _gnutls_x509_write_pubkey_params(pk_algorithm, params, &der);
	if (result < 0) {
		gnutls_assert();
		return result;
	}

	_asnstr_append_name(name, sizeof(name), dst_name,
			    ".algorithm.parameters");

	result = asn1_write_value(dst, name, der.data, der.size);
	_gnutls_free_datum(&der);

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = _gnutls_x509_write_pubkey(pk_algorithm, params, &der);
	if (result < 0) {
		gnutls_assert();
		return result;
	}

	/* Write the DER parameters. (in bits)
	 */
	_asnstr_append_name(name, sizeof(name), dst_name,
			    ".subjectPublicKey");
	result = asn1_write_value(dst, name, der.data, der.size * 8);
	_gnutls_free_datum(&der);

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	return 0;
}