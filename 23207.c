int __init efi_get_fdt_params(struct efi_fdt_params *params)
{
	struct param_info info;
	int ret;

	pr_info("Getting EFI parameters from FDT:\n");

	info.found = 0;
	info.params = params;

	ret = of_scan_flat_dt(fdt_find_uefi_params, &info);
	if (!info.found)
		pr_info("UEFI not found.\n");
	else if (!ret)
		pr_err("Can't find '%s' in device tree!\n",
		       info.missing);

	return ret;
}