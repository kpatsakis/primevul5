void xdp_umem_clear_dev(struct xdp_umem *umem)
{
	struct netdev_bpf bpf;
	int err;

	ASSERT_RTNL();

	if (!umem->dev)
		return;

	if (umem->zc) {
		bpf.command = XDP_SETUP_XSK_UMEM;
		bpf.xsk.umem = NULL;
		bpf.xsk.queue_id = umem->queue_id;

		err = umem->dev->netdev_ops->ndo_bpf(umem->dev, &bpf);

		if (err)
			WARN(1, "failed to disable umem!\n");
	}

	xdp_clear_umem_at_qid(umem->dev, umem->queue_id);

	dev_put(umem->dev);
	umem->dev = NULL;
	umem->zc = false;
}