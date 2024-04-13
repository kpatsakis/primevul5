static int xdp_umem_map_pages(struct xdp_umem *umem)
{
	unsigned int i;
	void *addr;

	for (i = 0; i < umem->npgs; i++) {
		if (PageHighMem(umem->pgs[i]))
			addr = vmap(&umem->pgs[i], 1, VM_MAP, PAGE_KERNEL);
		else
			addr = page_address(umem->pgs[i]);

		if (!addr) {
			xdp_umem_unmap_pages(umem);
			return -ENOMEM;
		}

		umem->pages[i].addr = addr;
	}

	return 0;
}