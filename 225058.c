static int dlpar_parse_resource(char **cmd, struct pseries_hp_errorlog *hp_elog)
{
	char *arg;

	arg = strsep(cmd, " ");
	if (!arg)
		return -EINVAL;

	if (sysfs_streq(arg, "memory")) {
		hp_elog->resource = PSERIES_HP_ELOG_RESOURCE_MEM;
	} else if (sysfs_streq(arg, "cpu")) {
		hp_elog->resource = PSERIES_HP_ELOG_RESOURCE_CPU;
	} else {
		pr_err("Invalid resource specified.\n");
		return -EINVAL;
	}

	return 0;
}