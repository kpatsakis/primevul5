time_t _gnutls_x509_get_time(ASN1_TYPE c2, const char *when, int nochoice)
{
	char ttime[MAX_TIME];
	char name[128];
	time_t c_time = (time_t) - 1;
	int len, result;

	len = sizeof(ttime) - 1;
	result = asn1_read_value(c2, when, ttime, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return (time_t) (-1);
	}

	if (nochoice != 0) {
		c_time = _gnutls_x509_generalTime2gtime(ttime);
	} else {
		_gnutls_str_cpy(name, sizeof(name), when);

		/* choice */
		if (strcmp(ttime, "generalTime") == 0) {
			_gnutls_str_cat(name, sizeof(name),
					".generalTime");
			len = sizeof(ttime) - 1;
			result = asn1_read_value(c2, name, ttime, &len);
			if (result == ASN1_SUCCESS)
				c_time =
				    _gnutls_x509_generalTime2gtime(ttime);
		} else {	/* UTCTIME */
			_gnutls_str_cat(name, sizeof(name), ".utcTime");
			len = sizeof(ttime) - 1;
			result = asn1_read_value(c2, name, ttime, &len);
			if (result == ASN1_SUCCESS)
				c_time = utcTime2gtime(ttime);
		}

		/* We cannot handle dates after 2031 in 32 bit machines.
		 * a time_t of 64bits has to be used.
		 */
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			return (time_t) (-1);
		}
	}

	return c_time;
}