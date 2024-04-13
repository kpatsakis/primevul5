const char *gnutls_x509_dn_oid_name(const char *oid, unsigned int flags)
{
	unsigned int i = 0;
	unsigned len = strlen(oid);

	do {
		if ((_oid2str[i].oid_size == len) && 
			strcmp(_oid2str[i].oid, oid) == 0 && _oid2str[i].ldap_desc != NULL)
			return _oid2str[i].ldap_desc;
		i++;
	}
	while (_oid2str[i].oid != NULL);

	if (flags & GNUTLS_X509_DN_OID_RETURN_OID)
		return oid;
	else
		return NULL;
}