static inline struct acpi_table_header *get_header(struct config_item *cfg)
{
	struct acpi_table *table = container_of(cfg, struct acpi_table, cfg);

	if (!table->header)
		pr_err("table not loaded\n");

	return table->header;
}