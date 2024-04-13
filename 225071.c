static int dlpar_parse_id_type(char **cmd, struct pseries_hp_errorlog *hp_elog)
{
	char *arg;
	u32 count, index;

	arg = strsep(cmd, " ");
	if (!arg)
		return -EINVAL;

	if (sysfs_streq(arg, "indexed-count")) {
		hp_elog->id_type = PSERIES_HP_ELOG_ID_DRC_IC;
		arg = strsep(cmd, " ");
		if (!arg) {
			pr_err("No DRC count specified.\n");
			return -EINVAL;
		}

		if (kstrtou32(arg, 0, &count)) {
			pr_err("Invalid DRC count specified.\n");
			return -EINVAL;
		}

		arg = strsep(cmd, " ");
		if (!arg) {
			pr_err("No DRC Index specified.\n");
			return -EINVAL;
		}

		if (kstrtou32(arg, 0, &index)) {
			pr_err("Invalid DRC Index specified.\n");
			return -EINVAL;
		}

		hp_elog->_drc_u.ic.count = cpu_to_be32(count);
		hp_elog->_drc_u.ic.index = cpu_to_be32(index);
	} else if (sysfs_streq(arg, "index")) {
		hp_elog->id_type = PSERIES_HP_ELOG_ID_DRC_INDEX;
		arg = strsep(cmd, " ");
		if (!arg) {
			pr_err("No DRC Index specified.\n");
			return -EINVAL;
		}

		if (kstrtou32(arg, 0, &index)) {
			pr_err("Invalid DRC Index specified.\n");
			return -EINVAL;
		}

		hp_elog->_drc_u.drc_index = cpu_to_be32(index);
	} else if (sysfs_streq(arg, "count")) {
		hp_elog->id_type = PSERIES_HP_ELOG_ID_DRC_COUNT;
		arg = strsep(cmd, " ");
		if (!arg) {
			pr_err("No DRC count specified.\n");
			return -EINVAL;
		}

		if (kstrtou32(arg, 0, &count)) {
			pr_err("Invalid DRC count specified.\n");
			return -EINVAL;
		}

		hp_elog->_drc_u.drc_count = cpu_to_be32(count);
	} else {
		pr_err("Invalid id_type specified.\n");
		return -EINVAL;
	}

	return 0;
}