static void xdp_umem_unmap_pages(struct xdp_umem *umem)
{
	unsigned int i;

	for (i = 0; i < umem->npgs; i++)
		if (PageHighMem(umem->pgs[i]))
			vunmap(umem->pages[i].addr);
}