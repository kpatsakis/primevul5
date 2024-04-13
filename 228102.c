int mnt_optstr_append_option(char **optstr, const char *name, const char *value)
{
	struct ul_buffer buf = UL_INIT_BUFFER;
	int rc;
	size_t nsz, vsz, osz;

	if (!optstr)
		return -EINVAL;
	if (!name || !*name)
		return 0;

	nsz = strlen(name);
	osz = *optstr ? strlen(*optstr) : 0;
	vsz = value ? strlen(value) : 0;

	ul_buffer_refer_string(&buf, *optstr);
	ul_buffer_set_chunksize(&buf, osz + nsz + vsz + 3);	/* to call realloc() only once */

	rc = __buffer_append_option(&buf, name, nsz, value, vsz);

	*optstr = ul_buffer_get_data(&buf, NULL, NULL);
	return rc;
}