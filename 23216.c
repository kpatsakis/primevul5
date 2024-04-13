static __init int efivar_ssdt_iter(efi_char16_t *name, efi_guid_t vendor,
				   unsigned long name_size, void *data)
{
	struct efivar_entry *entry;
	struct list_head *list = data;
	char utf8_name[EFIVAR_SSDT_NAME_MAX];
	int limit = min_t(unsigned long, EFIVAR_SSDT_NAME_MAX, name_size);

	ucs2_as_utf8(utf8_name, name, limit - 1);
	if (strncmp(utf8_name, efivar_ssdt, limit) != 0)
		return 0;

	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return 0;

	memcpy(entry->var.VariableName, name, name_size);
	memcpy(&entry->var.VendorGuid, &vendor, sizeof(efi_guid_t));

	efivar_entry_add(entry, list);

	return 0;
}