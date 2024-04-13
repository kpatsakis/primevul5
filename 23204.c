static int __init fdt_find_uefi_params(unsigned long node, const char *uname,
				       int depth, void *data)
{
	struct param_info *info = data;
	int i;

	for (i = 0; i < ARRAY_SIZE(dt_params); i++) {
		const char *subnode = dt_params[i].subnode;

		if (depth != 1 || strcmp(uname, dt_params[i].uname) != 0) {
			info->missing = dt_params[i].params[0].name;
			continue;
		}

		if (subnode) {
			int err = of_get_flat_dt_subnode_by_name(node, subnode);

			if (err < 0)
				return 0;

			node = err;
		}

		return __find_uefi_params(node, info, dt_params[i].params);
	}

	return 0;
}