static int test_apply(struct libmnt_test *ts, int argc, char *argv[])
{
	char *optstr;
	int rc, map;
	unsigned long flags;

	if (argc < 4)
		return -EINVAL;

	if (!strcmp(argv[1], "--user"))
		map = MNT_USERSPACE_MAP;
	else if (!strcmp(argv[1], "--linux"))
		map = MNT_LINUX_MAP;
	else {
		fprintf(stderr, "unknown option '%s'\n", argv[1]);
		return -EINVAL;
	}

	optstr = xstrdup(argv[2]);
	flags = strtoul(argv[3], NULL, 16);

	printf("flags:  0x%08lx\n", flags);

	rc = mnt_optstr_apply_flags(&optstr, flags, mnt_get_builtin_optmap(map));
	printf("optstr: %s\n", optstr);

	free(optstr);
	return rc;
}