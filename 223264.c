static void xdp_umem_unpin_pages(struct xdp_umem *umem)
{
	unpin_user_pages_dirty_lock(umem->pgs, umem->npgs, true);

	kfree(umem->pgs);
	umem->pgs = NULL;
}