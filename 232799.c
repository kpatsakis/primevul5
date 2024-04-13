_gnutls_x509_encode_PKI_params(gnutls_datum_t * der,
			       gnutls_pk_algorithm_t
			       pk_algorithm, gnutls_pk_params_st * params)
{
	int ret;
	ASN1_TYPE tmp;

	ret = asn1_create_element(_gnutls_get_pkix(),
				  "PKIX1.Certificate", &tmp);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	ret = _gnutls_x509_encode_and_copy_PKI_params(tmp,
						      "tbsCertificate.subjectPublicKeyInfo",
						      pk_algorithm,
						      params);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		goto cleanup;
	}

	ret =
	    _gnutls_x509_der_encode(tmp,
				    "tbsCertificate.subjectPublicKeyInfo",
				    der, 0);

      cleanup:
	asn1_delete_structure(&tmp);

	return ret;
}