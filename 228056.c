static int mnt_optstr_locate_option(char *optstr, const char *name,
					struct libmnt_optloc *ol)
{
	char *n;
	size_t namesz, nsz;
	int rc;

	if (!optstr)
		return 1;

	assert(name);

	namesz = strlen(name);

	do {
		rc = mnt_optstr_parse_next(&optstr, &n, &nsz,
					&ol->value, &ol->valsz);
		if (rc)
			break;

		if (namesz == nsz && strncmp(n, name, nsz) == 0) {
			ol->begin = n;
			ol->end = *(optstr - 1) == ',' ? optstr - 1 : optstr;
			ol->namesz = nsz;
			return 0;
		}
	} while(1);

	return rc;
}