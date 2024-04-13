static int test_split(struct libmnt_test *ts, int argc, char *argv[])
{
	char *optstr, *user = NULL, *fs = NULL, *vfs = NULL;
	int rc;

	if (argc < 2)
		return -EINVAL;

	optstr = xstrdup(argv[1]);

	rc = mnt_split_optstr(optstr, &user, &vfs, &fs, 0, 0);
	if (!rc) {
		printf("user : %s\n", user);
		printf("vfs  : %s\n", vfs);
		printf("fs   : %s\n", fs);
	}

	free(user);
	free(vfs);
	free(fs);
	free(optstr);
	return rc;
}