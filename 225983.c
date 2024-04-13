static ssize_t acpi_table_asl_compiler_id_show(struct config_item *cfg,
					       char *str)
{
	struct acpi_table_header *h = get_header(cfg);

	if (!h)
		return -EINVAL;

	return sprintf(str, "%.*s\n", ACPI_NAMESEG_SIZE, h->asl_compiler_id);
}