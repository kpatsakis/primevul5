gnutls_x509_subject_alt_name_t _gnutls_x509_san_find_type(char *str_type)
{
	if (strcmp(str_type, "dNSName") == 0)
		return GNUTLS_SAN_DNSNAME;
	if (strcmp(str_type, "rfc822Name") == 0)
		return GNUTLS_SAN_RFC822NAME;
	if (strcmp(str_type, "uniformResourceIdentifier") == 0)
		return GNUTLS_SAN_URI;
	if (strcmp(str_type, "iPAddress") == 0)
		return GNUTLS_SAN_IPADDRESS;
	if (strcmp(str_type, "otherName") == 0)
		return GNUTLS_SAN_OTHERNAME;
	if (strcmp(str_type, "directoryName") == 0)
		return GNUTLS_SAN_DN;
	return (gnutls_x509_subject_alt_name_t) - 1;
}