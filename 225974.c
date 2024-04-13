static ssize_t acpi_table_aml_read(struct config_item *cfg,
				   void *data, size_t size)
{
	struct acpi_table_header *h = get_header(cfg);

	if (!h)
		return -EINVAL;

	if (data)
		memcpy(data, h, h->length);

	return h->length;
}