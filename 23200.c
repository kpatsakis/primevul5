static int __init __find_uefi_params(unsigned long node,
				     struct param_info *info,
				     struct params *params)
{
	const void *prop;
	void *dest;
	u64 val;
	int i, len;

	for (i = 0; i < EFI_FDT_PARAMS_SIZE; i++) {
		prop = of_get_flat_dt_prop(node, params[i].propname, &len);
		if (!prop) {
			info->missing = params[i].name;
			return 0;
		}

		dest = info->params + params[i].offset;
		info->found++;

		val = of_read_number(prop, len / sizeof(u32));

		if (params[i].size == sizeof(u32))
			*(u32 *)dest = val;
		else
			*(u64 *)dest = val;

		if (efi_enabled(EFI_DBG))
			pr_info("  %s: 0x%0*llx\n", params[i].name,
				params[i].size * 2, val);
	}

	return 1;
}