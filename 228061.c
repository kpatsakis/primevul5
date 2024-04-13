int mnt_optstr_fix_gid(char **optstr, char *value, size_t valsz, char **next)
{
	char *end;

	if (!optstr || !*optstr || !value || !valsz)
		return -EINVAL;

	DBG(CXT, ul_debug("fixing gid"));

	end = value + valsz;

	if (valsz == 7 && !strncmp(value, "usergid", 7) &&
	    (*(value + 7) == ',' || !*(value + 7)))
		return set_uint_value(optstr, getgid(), value, end, next);

	if (!isdigit(*value)) {
		int rc;
		gid_t id;
		char *p = strndup(value, valsz);
		if (!p)
			return -ENOMEM;
		rc = mnt_get_gid(p, &id);
		free(p);

		if (!rc)
			return set_uint_value(optstr, id, value, end, next);

	}

	if (next) {
		/* nothing */
		*next = value + valsz;
		if (**next == ',')
			(*next)++;
	}
	return 0;
}