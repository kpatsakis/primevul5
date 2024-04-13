int efi_mem_desc_lookup(u64 phys_addr, efi_memory_desc_t *out_md)
{
	efi_memory_desc_t *md;

	if (!efi_enabled(EFI_MEMMAP)) {
		pr_err_once("EFI_MEMMAP is not enabled.\n");
		return -EINVAL;
	}

	if (!out_md) {
		pr_err_once("out_md is null.\n");
		return -EINVAL;
        }

	for_each_efi_memory_desc(md) {
		u64 size;
		u64 end;

		size = md->num_pages << EFI_PAGE_SHIFT;
		end = md->phys_addr + size;
		if (phys_addr >= md->phys_addr && phys_addr < end) {
			memcpy(out_md, md, sizeof(*out_md));
			return 0;
		}
	}
	return -ENOENT;
}