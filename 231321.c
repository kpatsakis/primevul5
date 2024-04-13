int trace_run_command(const char *buf, int (*createfn)(int, char **))
{
	char **argv;
	int argc, ret;

	argc = 0;
	ret = 0;
	argv = argv_split(GFP_KERNEL, buf, &argc);
	if (!argv)
		return -ENOMEM;

	if (argc)
		ret = createfn(argc, argv);

	argv_free(argv);

	return ret;
}