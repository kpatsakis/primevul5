static const struct oid_to_string *get_oid_entry(const char *oid)
{
	unsigned int i = 0;
	unsigned len = strlen(oid);

	do {
		if (len == _oid2str[i].oid_size &&
			strcmp(_oid2str[i].oid, oid) == 0)
			return &_oid2str[i];
		i++;
	}
	while (_oid2str[i].oid != NULL);

	return NULL;
}