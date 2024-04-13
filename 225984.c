static ssize_t acpi_table_oem_table_id_show(struct config_item *cfg, char *str)
{
	struct acpi_table_header *h = get_header(cfg);

	if (!h)
		return -EINVAL;

	return sprintf(str, "%.*s\n", ACPI_OEM_TABLE_ID_SIZE, h->oem_table_id);
}