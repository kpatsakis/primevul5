int mnt_optstr_fix_secontext(char **optstr,
			     char *value,
			     size_t valsz,
			     char **next)
{
	int rc = 0;
	char *p, *val, *begin, *end, *raw = NULL;
	size_t sz;

	if (!optstr || !*optstr || !value || !valsz)
		return -EINVAL;

	DBG(CXT, ul_debug("fixing SELinux context"));

	begin = value;
	end = value + valsz;

	/* the selinux contexts are quoted */
	if (*value == '"') {
		if (valsz <= 2 || *(value + valsz - 1) != '"')
			return -EINVAL;		/* improperly quoted option string */
		value++;
		valsz -= 2;
	}

	p = strndup(value, valsz);
	if (!p)
		return -ENOMEM;


	/* translate the context */
	rc = selinux_trans_to_raw_context(p, &raw);

	DBG(CXT, ul_debug("SELinux context '%s' translated to '%s'",
			p, rc == -1 ? "FAILED" : (char *) raw));

	free(p);
	if (rc == -1 ||	!raw)
		return -EINVAL;


	/* create a quoted string from the raw context */
	sz = strlen((char *) raw);
	if (!sz)
		return -EINVAL;

	p = val = malloc(valsz + 3);
	if (!val)
		return -ENOMEM;

	*p++ = '"';
	memcpy(p, raw, sz);
	p += sz;
	*p++ = '"';
	*p = '\0';

	freecon(raw);

	/* set new context */
	mnt_optstr_remove_option_at(optstr, begin, end);
	rc = insert_value(optstr, begin, val, next);
	free(val);

	return rc;
}