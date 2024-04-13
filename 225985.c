static ssize_t acpi_table_oem_id_show(struct config_item *cfg, char *str)
{
	struct acpi_table_header *h = get_header(cfg);

	if (!h)
		return -EINVAL;

	return sprintf(str, "%.*s\n", ACPI_OEM_ID_SIZE, h->oem_id);
}