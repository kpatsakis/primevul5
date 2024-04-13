_gnutls_x509_set_time(ASN1_TYPE c2, const char *where, time_t tim,
		      int nochoice)
{
	char str_time[MAX_TIME];
	char name[128];
	int result, len;

	if (nochoice != 0) {
		result =
		    gtime2generalTime(tim, str_time, sizeof(str_time));
		if (result < 0)
			return gnutls_assert_val(result);
		len = strlen(str_time);
		result = asn1_write_value(c2, where, str_time, len);
		if (result != ASN1_SUCCESS)
			return gnutls_assert_val(_gnutls_asn2err(result));

		return 0;
	}

	_gnutls_str_cpy(name, sizeof(name), where);

	if ((result = asn1_write_value(c2, name, "generalTime", 1)) < 0) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = gtime2generalTime(tim, str_time, sizeof(str_time));
	if (result < 0) {
		gnutls_assert();
		return result;
	}

	_gnutls_str_cat(name, sizeof(name), ".generalTime");

	len = strlen(str_time);
	result = asn1_write_value(c2, name, str_time, len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	return 0;
}