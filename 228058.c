static int test_prepend(struct libmnt_test *ts, int argc, char *argv[])
{
	const char *value = NULL, *name;
	char *optstr;
	int rc;

	if (argc < 3)
		return -EINVAL;
	optstr = xstrdup(argv[1]);
	name = argv[2];

	if (argc == 4)
		value = argv[3];

	rc = mnt_optstr_prepend_option(&optstr, name, value);
	if (!rc)
		printf("result: >%s<\n", optstr);
	free(optstr);
	return rc;
}