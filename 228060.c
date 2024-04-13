int mnt_optstr_deduplicate_option(char **optstr, const char *name)
{
	int rc;
	char *begin = NULL, *end = NULL, *opt;

	if (!optstr || !name)
		return -EINVAL;

	opt = *optstr;
	do {
		struct libmnt_optloc ol = MNT_INIT_OPTLOC;

		rc = mnt_optstr_locate_option(opt, name, &ol);
		if (!rc) {
			if (begin) {
				/* remove the previous instance */
				size_t shift = strlen(*optstr);

				mnt_optstr_remove_option_at(optstr, begin, end);

				/* now all the offsets are not valid anymore - recount */
				shift -= strlen(*optstr);
				ol.begin -= shift;
				ol.end -= shift;
			}
			begin = ol.begin;
			end = ol.end;
			opt = end && *end ? end + 1 : NULL;
		}
		if (opt == NULL)
			break;
	} while (rc == 0 && *opt);

	return rc < 0 ? rc : begin ? 0 : 1;
}