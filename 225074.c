static int dlpar_parse_action(char **cmd, struct pseries_hp_errorlog *hp_elog)
{
	char *arg;

	arg = strsep(cmd, " ");
	if (!arg)
		return -EINVAL;

	if (sysfs_streq(arg, "add")) {
		hp_elog->action = PSERIES_HP_ELOG_ACTION_ADD;
	} else if (sysfs_streq(arg, "remove")) {
		hp_elog->action = PSERIES_HP_ELOG_ACTION_REMOVE;
	} else {
		pr_err("Invalid action specified.\n");
		return -EINVAL;
	}

	return 0;
}