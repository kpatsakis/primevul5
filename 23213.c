static __init int efivar_ssdt_load(void)
{
	LIST_HEAD(entries);
	struct efivar_entry *entry, *aux;
	unsigned long size;
	void *data;
	int ret;

	ret = efivar_init(efivar_ssdt_iter, &entries, true, &entries);

	list_for_each_entry_safe(entry, aux, &entries, list) {
		pr_info("loading SSDT from variable %s-%pUl\n", efivar_ssdt,
			&entry->var.VendorGuid);

		list_del(&entry->list);

		ret = efivar_entry_size(entry, &size);
		if (ret) {
			pr_err("failed to get var size\n");
			goto free_entry;
		}

		data = kmalloc(size, GFP_KERNEL);
		if (!data) {
			ret = -ENOMEM;
			goto free_entry;
		}

		ret = efivar_entry_get(entry, NULL, &size, data);
		if (ret) {
			pr_err("failed to get var data\n");
			goto free_data;
		}

		ret = acpi_load_table(data);
		if (ret) {
			pr_err("failed to load table: %d\n", ret);
			goto free_data;
		}

		goto free_entry;

free_data:
		kfree(data);

free_entry:
		kfree(entry);
	}

	return ret;
}