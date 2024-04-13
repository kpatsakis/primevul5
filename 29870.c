const char *ldb_dn_get_linearized(struct ldb_dn *dn)
{
	unsigned int i;
	size_t len;
	char *d, *n;

	if ( ! dn || ( dn->invalid)) return NULL;

	if (dn->linearized) return dn->linearized;

	if ( ! dn->components) {
		ldb_dn_mark_invalid(dn);
		return NULL;
	}

	if (dn->comp_num == 0) {
		dn->linearized = talloc_strdup(dn, "");
		if ( ! dn->linearized) return NULL;
		return dn->linearized;
	}

	/* calculate maximum possible length of DN */
	for (len = 0, i = 0; i < dn->comp_num; i++) {
		/* name len */
		len += strlen(dn->components[i].name);
		/* max escaped data len */
		len += (dn->components[i].value.length * 3);
		len += 2; /* '=' and ',' */
	}
	dn->linearized = talloc_array(dn, char, len);
	if ( ! dn->linearized) return NULL;

	d = dn->linearized;

	for (i = 0; i < dn->comp_num; i++) {

		/* copy the name */
		n = dn->components[i].name;
		while (*n) *d++ = *n++;

		*d++ = '=';

		/* and the value */
		d += ldb_dn_escape_internal( d,
				(char *)dn->components[i].value.data,
				dn->components[i].value.length);
		*d++ = ',';
	}

	*(--d) = '\0';

	/* don't waste more memory than necessary */
	dn->linearized = talloc_realloc(dn, dn->linearized,
					char, (d - dn->linearized + 1));

	return dn->linearized;
}
