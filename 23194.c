int efi_mem_type(unsigned long phys_addr)
{
	const efi_memory_desc_t *md;

	if (!efi_enabled(EFI_MEMMAP))
		return -ENOTSUPP;

	for_each_efi_memory_desc(md) {
		if ((md->phys_addr <= phys_addr) &&
		    (phys_addr < (md->phys_addr +
				  (md->num_pages << EFI_PAGE_SHIFT))))
			return md->type;
	}
	return -EINVAL;
}