static __init int match_config_table(efi_guid_t *guid,
				     unsigned long table,
				     efi_config_table_type_t *table_types)
{
	int i;

	if (table_types) {
		for (i = 0; efi_guidcmp(table_types[i].guid, NULL_GUID); i++) {
			if (!efi_guidcmp(*guid, table_types[i].guid)) {
				*(table_types[i].ptr) = table;
				if (table_types[i].name)
					pr_cont(" %s=0x%lx ",
						table_types[i].name, table);
				return 1;
			}
		}
	}

	return 0;
}