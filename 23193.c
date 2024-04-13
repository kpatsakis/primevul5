int __init efi_config_parse_tables(void *config_tables, int count, int sz,
				   efi_config_table_type_t *arch_tables)
{
	void *tablep;
	int i;

	tablep = config_tables;
	pr_info("");
	for (i = 0; i < count; i++) {
		efi_guid_t guid;
		unsigned long table;

		if (efi_enabled(EFI_64BIT)) {
			u64 table64;
			guid = ((efi_config_table_64_t *)tablep)->guid;
			table64 = ((efi_config_table_64_t *)tablep)->table;
			table = table64;
#ifndef CONFIG_64BIT
			if (table64 >> 32) {
				pr_cont("\n");
				pr_err("Table located above 4GB, disabling EFI.\n");
				return -EINVAL;
			}
#endif
		} else {
			guid = ((efi_config_table_32_t *)tablep)->guid;
			table = ((efi_config_table_32_t *)tablep)->table;
		}

		if (!match_config_table(&guid, table, common_tables))
			match_config_table(&guid, table, arch_tables);

		tablep += sz;
	}
	pr_cont("\n");
	set_bit(EFI_CONFIG_TABLES, &efi.flags);

	if (efi.rng_seed != EFI_INVALID_TABLE_ADDR) {
		struct linux_efi_random_seed *seed;
		u32 size = 0;

		seed = early_memremap(efi.rng_seed, sizeof(*seed));
		if (seed != NULL) {
			size = seed->size;
			early_memunmap(seed, sizeof(*seed));
		} else {
			pr_err("Could not map UEFI random seed!\n");
		}
		if (size > 0) {
			seed = early_memremap(efi.rng_seed,
					      sizeof(*seed) + size);
			if (seed != NULL) {
				pr_notice("seeding entropy pool\n");
				add_device_randomness(seed->bits, seed->size);
				early_memunmap(seed, sizeof(*seed) + size);
			} else {
				pr_err("Could not map UEFI random seed!\n");
			}
		}
	}

	if (efi_enabled(EFI_MEMMAP))
		efi_memattr_init();

	efi_tpm_eventlog_init();

	/* Parse the EFI Properties table if it exists */
	if (efi.properties_table != EFI_INVALID_TABLE_ADDR) {
		efi_properties_table_t *tbl;

		tbl = early_memremap(efi.properties_table, sizeof(*tbl));
		if (tbl == NULL) {
			pr_err("Could not map Properties table!\n");
			return -ENOMEM;
		}

		if (tbl->memory_protection_attribute &
		    EFI_PROPERTIES_RUNTIME_MEMORY_PROTECTION_NON_EXECUTABLE_PE_DATA)
			set_bit(EFI_NX_PE_DATA, &efi.flags);

		early_memunmap(tbl, sizeof(*tbl));
	}

	if (efi.mem_reserve != EFI_INVALID_TABLE_ADDR) {
		unsigned long prsv = efi.mem_reserve;

		while (prsv) {
			struct linux_efi_memreserve *rsv;
			u8 *p;
			int i;

			/*
			 * Just map a full page: that is what we will get
			 * anyway, and it permits us to map the entire entry
			 * before knowing its size.
			 */
			p = early_memremap(ALIGN_DOWN(prsv, PAGE_SIZE),
					   PAGE_SIZE);
			if (p == NULL) {
				pr_err("Could not map UEFI memreserve entry!\n");
				return -ENOMEM;
			}

			rsv = (void *)(p + prsv % PAGE_SIZE);

			/* reserve the entry itself */
			memblock_reserve(prsv, EFI_MEMRESERVE_SIZE(rsv->size));

			for (i = 0; i < atomic_read(&rsv->count); i++) {
				memblock_reserve(rsv->entry[i].base,
						 rsv->entry[i].size);
			}

			prsv = rsv->next;
			early_memunmap(p, PAGE_SIZE);
		}
	}

	return 0;
}