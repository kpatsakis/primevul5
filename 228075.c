int mnt_optstr_get_option(const char *optstr, const char *name,
			  char **value, size_t *valsz)
{
	struct libmnt_optloc ol = MNT_INIT_OPTLOC;
	int rc;

	if (!optstr || !name)
		return -EINVAL;

	rc = mnt_optstr_locate_option((char *) optstr, name, &ol);
	if (!rc) {
		if (value)
			*value = ol.value;
		if (valsz)
			*valsz = ol.valsz;
	}
	return rc;
}