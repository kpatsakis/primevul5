int mnt_optstr_set_option(char **optstr, const char *name, const char *value)
{
	struct libmnt_optloc ol = MNT_INIT_OPTLOC;
	char *nameend;
	int rc = 1;

	if (!optstr || !name)
		return -EINVAL;

	if (*optstr)
		rc = mnt_optstr_locate_option(*optstr, name, &ol);
	if (rc < 0)
		return rc;			/* parse error */
	if (rc == 1)
		return mnt_optstr_append_option(optstr, name, value);	/* not found */

	nameend = ol.begin + ol.namesz;

	if (value == NULL && ol.value && ol.valsz)
		/* remove unwanted "=value" */
		mnt_optstr_remove_option_at(optstr, nameend, ol.end);

	else if (value && ol.value == NULL)
		/* insert "=value" */
		rc = insert_value(optstr, nameend, value, NULL);

	else if (value && ol.value && strlen(value) == ol.valsz)
		/* simply replace =value */
		memcpy(ol.value, value, ol.valsz);

	else if (value && ol.value) {
		mnt_optstr_remove_option_at(optstr, nameend, ol.end);
		rc = insert_value(optstr, nameend, value, NULL);
	}
	return rc;
}