int __ref efi_mem_reserve_persistent(phys_addr_t addr, u64 size)
{
	struct linux_efi_memreserve *rsv;
	unsigned long prsv;
	int rc, index;

	if (efi_memreserve_root == (void *)ULONG_MAX)
		return -ENODEV;

	if (!efi_memreserve_root) {
		rc = efi_memreserve_map_root();
		if (rc)
			return rc;
	}

	/* first try to find a slot in an existing linked list entry */
	for (prsv = efi_memreserve_root->next; prsv; prsv = rsv->next) {
		rsv = memremap(prsv, sizeof(*rsv), MEMREMAP_WB);
		index = atomic_fetch_add_unless(&rsv->count, 1, rsv->size);
		if (index < rsv->size) {
			rsv->entry[index].base = addr;
			rsv->entry[index].size = size;

			memunmap(rsv);
			return 0;
		}
		memunmap(rsv);
	}

	/* no slot found - allocate a new linked list entry */
	rsv = (struct linux_efi_memreserve *)__get_free_page(GFP_ATOMIC);
	if (!rsv)
		return -ENOMEM;

	/*
	 * The memremap() call above assumes that a linux_efi_memreserve entry
	 * never crosses a page boundary, so let's ensure that this remains true
	 * even when kexec'ing a 4k pages kernel from a >4k pages kernel, by
	 * using SZ_4K explicitly in the size calculation below.
	 */
	rsv->size = EFI_MEMRESERVE_COUNT(SZ_4K);
	atomic_set(&rsv->count, 1);
	rsv->entry[0].base = addr;
	rsv->entry[0].size = size;

	spin_lock(&efi_mem_reserve_persistent_lock);
	rsv->next = efi_memreserve_root->next;
	efi_memreserve_root->next = __pa(rsv);
	spin_unlock(&efi_mem_reserve_persistent_lock);

	return 0;
}