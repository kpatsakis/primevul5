u64 efi_mem_attributes(unsigned long phys_addr)
{
	efi_memory_desc_t *md;

	if (!efi_enabled(EFI_MEMMAP))
		return 0;

	for_each_efi_memory_desc(md) {
		if ((md->phys_addr <= phys_addr) &&
		    (phys_addr < (md->phys_addr +
		    (md->num_pages << EFI_PAGE_SHIFT))))
			return md->attribute;
	}
	return 0;
}