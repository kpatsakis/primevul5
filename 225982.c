static void acpi_table_drop_item(struct config_group *group,
				 struct config_item *cfg)
{
	struct acpi_table *table = container_of(cfg, struct acpi_table, cfg);

	ACPI_INFO(("Host-directed Dynamic ACPI Table Unload"));
	acpi_unload_table(table->index);
}