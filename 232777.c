decode_complex_string(const struct oid_to_string *oentry, void *value,
		      int value_size, gnutls_datum_t * out)
{
	char str[MAX_STRING_LEN], tmpname[128];
	int len = -1, result;
	ASN1_TYPE tmpasn = ASN1_TYPE_EMPTY;
	char asn1_err[ASN1_MAX_ERROR_DESCRIPTION_SIZE] = "";
	unsigned int etype;
	gnutls_datum_t td = {NULL, 0};

	if (oentry->asn_desc == NULL) {
		gnutls_assert();
		return GNUTLS_E_INTERNAL_ERROR;
	}

	if ((result =
	     asn1_create_element(_gnutls_get_pkix(), oentry->asn_desc,
				 &tmpasn)) != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	if ((result =
	     asn1_der_decoding(&tmpasn, value, value_size,
			       asn1_err)) != ASN1_SUCCESS) {
		gnutls_assert();
		_gnutls_debug_log("asn1_der_decoding: %s\n", asn1_err);
		asn1_delete_structure(&tmpasn);
		return _gnutls_asn2err(result);
	}

	/* Read the type of choice.
	 */
	len = sizeof(str) - 1;
	if ((result = asn1_read_value(tmpasn, "", str, &len)) != ASN1_SUCCESS) {	/* CHOICE */
		gnutls_assert();
		asn1_delete_structure(&tmpasn);
		return _gnutls_asn2err(result);
	}

	str[len] = 0;

	/* We set the etype on the strings that may need
	 * some conversion to UTF-8. The INVALID flag indicates
	 * no conversion needed */
	if (strcmp(str, "teletexString") == 0)
		etype = ASN1_ETYPE_TELETEX_STRING;
	else if (strcmp(str, "bmpString") == 0)
		etype = ASN1_ETYPE_BMP_STRING;
	else if (strcmp(str, "universalString") == 0)
		etype = ASN1_ETYPE_UNIVERSAL_STRING;
	else
		etype = ASN1_ETYPE_INVALID;

	_gnutls_str_cpy(tmpname, sizeof(tmpname), str);

	result = _gnutls_x509_read_value(tmpasn, tmpname, &td);
	asn1_delete_structure(&tmpasn);
	if (result < 0)
		return gnutls_assert_val(result);

	if (etype != ASN1_ETYPE_INVALID) {
		result = make_printable_string(etype, &td, out);

		_gnutls_free_datum(&td);

		if (result < 0)
			return gnutls_assert_val(result);
	} else {
		out->data = td.data;
		out->size = td.size;
		/* _gnutls_x509_read_value always null terminates */
	}

	/* Refuse to deal with strings containing NULs. */
	if (strlen((void *) out->data) != (size_t) out->size) {
		_gnutls_free_datum(out);
		return gnutls_assert_val(GNUTLS_E_ASN1_DER_ERROR);
	}

	return 0;
}