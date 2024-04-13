static struct config_item *acpi_table_make_item(struct config_group *group,
						const char *name)
{
	struct acpi_table *table;

	table = kzalloc(sizeof(*table), GFP_KERNEL);
	if (!table)
		return ERR_PTR(-ENOMEM);

	config_item_init_type_name(&table->cfg, name, &acpi_table_type);
	return &table->cfg;
}