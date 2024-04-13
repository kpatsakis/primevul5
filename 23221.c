int __init efi_config_init(efi_config_table_type_t *arch_tables)
{
	void *config_tables;
	int sz, ret;

	if (efi.systab->nr_tables == 0)
		return 0;

	if (efi_enabled(EFI_64BIT))
		sz = sizeof(efi_config_table_64_t);
	else
		sz = sizeof(efi_config_table_32_t);

	/*
	 * Let's see what config tables the firmware passed to us.
	 */
	config_tables = early_memremap(efi.systab->tables,
				       efi.systab->nr_tables * sz);
	if (config_tables == NULL) {
		pr_err("Could not map Configuration table!\n");
		return -ENOMEM;
	}

	ret = efi_config_parse_tables(config_tables, efi.systab->nr_tables, sz,
				      arch_tables);

	early_memunmap(config_tables, efi.systab->nr_tables * sz);
	return ret;
}