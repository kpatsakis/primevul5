_gnutls_x509_encode_and_write_attribute(const char *given_oid,
					ASN1_TYPE asn1_struct,
					const char *where,
					const void *_data,
					int data_size, int multi)
{
	const uint8_t *data = _data;
	char tmp[128];
	int result;
	const struct oid_to_string *oentry;

	oentry = get_oid_entry(given_oid);
	if (oentry == NULL) {
		gnutls_assert();
		_gnutls_debug_log("Cannot find OID: %s\n", given_oid);
		return GNUTLS_E_X509_UNSUPPORTED_OID;
	}

	/* write the data (value)
	 */

	_gnutls_str_cpy(tmp, sizeof(tmp), where);
	_gnutls_str_cat(tmp, sizeof(tmp), ".value");

	if (multi != 0) {	/* if not writing an AttributeTypeAndValue, but an Attribute */
		_gnutls_str_cat(tmp, sizeof(tmp), "s");	/* values */

		result = asn1_write_value(asn1_struct, tmp, "NEW", 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto error;
		}

		_gnutls_str_cat(tmp, sizeof(tmp), ".?LAST");
	}

	if (oentry->asn_desc != NULL) {	/* write a complex string API */
		result =
		    write_complex_string(asn1_struct, tmp, oentry, data,
					 data_size);
		if (result < 0)
			return gnutls_assert_val(result);
	} else {		/* write a simple string */

		gnutls_datum_t td;

		td.data = (void *) data;
		td.size = data_size;
		result =
		    _gnutls_x509_write_string(asn1_struct, tmp, &td,
					      oentry->etype);
		if (result < 0) {
			gnutls_assert();
			goto error;
		}
	}

	/* write the type
	 */
	_gnutls_str_cpy(tmp, sizeof(tmp), where);
	_gnutls_str_cat(tmp, sizeof(tmp), ".type");

	result = asn1_write_value(asn1_struct, tmp, given_oid, 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	result = 0;

      error:
	return result;
}