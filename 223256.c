void xdp_put_umem(struct xdp_umem *umem)
{
	if (!umem)
		return;

	if (refcount_dec_and_test(&umem->users)) {
		INIT_WORK(&umem->work, xdp_umem_release_deferred);
		schedule_work(&umem->work);
	}
}