int gnutls_x509_dn_oid_known(const char *oid)
{
	unsigned int i = 0;
	unsigned len = strlen(oid);

	do {
		if (len == _oid2str[i].oid_size &&
			strcmp(_oid2str[i].oid, oid) == 0)
			return 1;
		i++;
	}
	while (_oid2str[i].oid != NULL);

	return 0;
}