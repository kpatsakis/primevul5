int xdp_umem_assign_dev(struct xdp_umem *umem, struct net_device *dev,
			u16 queue_id, u16 flags)
{
	bool force_zc, force_copy;
	struct netdev_bpf bpf;
	int err = 0;

	ASSERT_RTNL();

	force_zc = flags & XDP_ZEROCOPY;
	force_copy = flags & XDP_COPY;

	if (force_zc && force_copy)
		return -EINVAL;

	if (xdp_get_umem_from_qid(dev, queue_id))
		return -EBUSY;

	err = xdp_reg_umem_at_qid(dev, umem, queue_id);
	if (err)
		return err;

	umem->dev = dev;
	umem->queue_id = queue_id;

	if (flags & XDP_USE_NEED_WAKEUP) {
		umem->flags |= XDP_UMEM_USES_NEED_WAKEUP;
		/* Tx needs to be explicitly woken up the first time.
		 * Also for supporting drivers that do not implement this
		 * feature. They will always have to call sendto().
		 */
		xsk_set_tx_need_wakeup(umem);
	}

	dev_hold(dev);

	if (force_copy)
		/* For copy-mode, we are done. */
		return 0;

	if (!dev->netdev_ops->ndo_bpf || !dev->netdev_ops->ndo_xsk_wakeup) {
		err = -EOPNOTSUPP;
		goto err_unreg_umem;
	}

	bpf.command = XDP_SETUP_XSK_UMEM;
	bpf.xsk.umem = umem;
	bpf.xsk.queue_id = queue_id;

	err = dev->netdev_ops->ndo_bpf(dev, &bpf);
	if (err)
		goto err_unreg_umem;

	umem->zc = true;
	return 0;

err_unreg_umem:
	if (!force_zc)
		err = 0; /* fallback to copy mode */
	if (err)
		xdp_clear_umem_at_qid(dev, queue_id);
	return err;
}