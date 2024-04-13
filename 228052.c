static int test_remove(struct libmnt_test *ts, int argc, char *argv[])
{
	const char *name;
	char *optstr;
	int rc;

	if (argc < 3)
		return -EINVAL;
	optstr = xstrdup(argv[1]);
	name = argv[2];

	rc = mnt_optstr_remove_option(&optstr, name);
	if (!rc)
		printf("result: >%s<\n", optstr);
	free(optstr);
	return rc;
}