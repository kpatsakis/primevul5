static void __exit acpi_configfs_exit(void)
{
	configfs_unregister_default_group(acpi_table_group);
	configfs_unregister_subsystem(&acpi_configfs);
}