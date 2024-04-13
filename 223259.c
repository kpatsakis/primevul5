bool xdp_umem_validate_queues(struct xdp_umem *umem)
{
	return umem->fq && umem->cq;
}