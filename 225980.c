static int __init acpi_configfs_init(void)
{
	int ret;
	struct config_group *root = &acpi_configfs.su_group;

	config_group_init(root);

	ret = configfs_register_subsystem(&acpi_configfs);
	if (ret)
		return ret;

	acpi_table_group = configfs_register_default_group(root, "table",
							   &acpi_tables_type);
	return PTR_ERR_OR_ZERO(acpi_table_group);
}