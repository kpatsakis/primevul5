static ssize_t acpi_table_asl_compiler_revision_show(struct config_item *cfg,
						     char *str)
{
	struct acpi_table_header *h = get_header(cfg);

	if (!h)
		return -EINVAL;

	return sprintf(str, "%d\n", h->asl_compiler_revision);
}