int mnt_optstr_prepend_option(char **optstr, const char *name, const char *value)
{
	struct ul_buffer buf = UL_INIT_BUFFER;
	size_t nsz, vsz, osz;
	int rc;

	if (!optstr)
		return -EINVAL;
	if (!name || !*name)
		return 0;

	nsz = strlen(name);
	osz = *optstr ? strlen(*optstr) : 0;
	vsz = value ? strlen(value) : 0;

	ul_buffer_set_chunksize(&buf, osz + nsz + vsz + 3);   /* to call realloc() only once */

	rc = __buffer_append_option(&buf, name, nsz, value, vsz);
	if (*optstr && !rc) {
		rc = ul_buffer_append_data(&buf, ",", 1);
		if (!rc)
			rc = ul_buffer_append_data(&buf, *optstr, osz);
		free(*optstr);
	}

	*optstr = ul_buffer_get_data(&buf, NULL, NULL);
	return rc;
}