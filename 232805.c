static int write_complex_string(ASN1_TYPE asn_struct, const char *where,
				const struct oid_to_string *oentry,
				const uint8_t * data, size_t data_size)
{
	char tmp[128];
	ASN1_TYPE c2;
	int result;
	const char *string_type;
	unsigned int i;

	result =
	    asn1_create_element(_gnutls_get_pkix(), oentry->asn_desc, &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	tmp[0] = 0;

	string_type = "printableString";

	/* Check if the data is ASN.1 printable, and use
	 * the UTF8 string type if not.
	 */
	for (i = 0; i < data_size; i++) {
		if (!is_printable(data[i])) {
			string_type = "utf8String";
			break;
		}
	}

	/* if the type is a CHOICE then write the
	 * type we'll use.
	 */
	result = asn1_write_value(c2, "", string_type, 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	_gnutls_str_cpy(tmp, sizeof(tmp), string_type);

	result = asn1_write_value(c2, tmp, data, data_size);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	result =
	    _gnutls_x509_der_encode_and_copy(c2, "", asn_struct, where, 0);
	if (result < 0) {
		gnutls_assert();
		goto error;
	}

	result = 0;

      error:
	asn1_delete_structure(&c2);
	return result;
}