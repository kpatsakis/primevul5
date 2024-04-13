const char *_gnutls_ldap_string_to_oid(const char *str, unsigned str_len)
{
	unsigned int i = 0;

	do {
		if ((_oid2str[i].ldap_desc != NULL) &&
		    (str_len == _oid2str[i].ldap_desc_size) &&
		    (strncasecmp(_oid2str[i].ldap_desc, str, str_len) ==
		     0))
			return _oid2str[i].oid;
		i++;
	}
	while (_oid2str[i].oid != NULL);

	return NULL;
}