static void xdp_umem_release(struct xdp_umem *umem)
{
	rtnl_lock();
	xdp_umem_clear_dev(umem);
	rtnl_unlock();

	ida_simple_remove(&umem_ida, umem->id);

	if (umem->fq) {
		xskq_destroy(umem->fq);
		umem->fq = NULL;
	}

	if (umem->cq) {
		xskq_destroy(umem->cq);
		umem->cq = NULL;
	}

	xsk_reuseq_destroy(umem);

	xdp_umem_unmap_pages(umem);
	xdp_umem_unpin_pages(umem);

	kvfree(umem->pages);
	umem->pages = NULL;

	xdp_umem_unaccount_pages(umem);
	kfree(umem);
}