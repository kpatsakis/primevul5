static int test_flags(struct libmnt_test *ts, int argc, char *argv[])
{
	char *optstr;
	int rc;
	unsigned long fl = 0;

	if (argc < 2)
		return -EINVAL;

	optstr = xstrdup(argv[1]);

	rc = mnt_optstr_get_flags(optstr, &fl, mnt_get_builtin_optmap(MNT_LINUX_MAP));
	if (rc)
		return rc;
	printf("mountflags:           0x%08lx\n", fl);

	fl = 0;
	rc = mnt_optstr_get_flags(optstr, &fl, mnt_get_builtin_optmap(MNT_USERSPACE_MAP));
	if (rc)
		return rc;
	printf("userspace-mountflags: 0x%08lx\n", fl);

	free(optstr);
	return rc;
}