int mnt_optstr_fix_uid(char **optstr, char *value, size_t valsz, char **next)
{
	char *end;

	if (!optstr || !*optstr || !value || !valsz)
		return -EINVAL;

	DBG(CXT, ul_debug("fixing uid"));

	end = value + valsz;

	if (valsz == 7 && !strncmp(value, "useruid", 7) &&
	    (*(value + 7) == ',' || !*(value + 7)))
		return set_uint_value(optstr, getuid(), value, end, next);

	if (!isdigit(*value)) {
		uid_t id;
		int rc;
		char *p = strndup(value, valsz);
		if (!p)
			return -ENOMEM;
		rc = mnt_get_uid(p, &id);
		free(p);

		if (!rc)
			return set_uint_value(optstr, id, value, end, next);
	}

	if (next) {
		/* no change, let's keep the original value */
		*next = value + valsz;
		if (**next == ',')
			(*next)++;
	}

	return 0;
}