static int xennet_get_extras(struct netfront_queue *queue,
			     struct xen_netif_extra_info *extras,
			     RING_IDX rp)

{
	struct xen_netif_extra_info extra;
	struct device *dev = &queue->info->netdev->dev;
	RING_IDX cons = queue->rx.rsp_cons;
	int err = 0;

	do {
		struct sk_buff *skb;
		grant_ref_t ref;

		if (unlikely(cons + 1 == rp)) {
			if (net_ratelimit())
				dev_warn(dev, "Missing extra info\n");
			err = -EBADR;
			break;
		}

		RING_COPY_RESPONSE(&queue->rx, ++cons, &extra);

		if (unlikely(!extra.type ||
			     extra.type >= XEN_NETIF_EXTRA_TYPE_MAX)) {
			if (net_ratelimit())
				dev_warn(dev, "Invalid extra type: %d\n",
					 extra.type);
			err = -EINVAL;
		} else {
			extras[extra.type - 1] = extra;
		}

		skb = xennet_get_rx_skb(queue, cons);
		ref = xennet_get_rx_ref(queue, cons);
		xennet_move_rx_slot(queue, skb, ref);
	} while (extra.flags & XEN_NETIF_EXTRA_FLAG_MORE);

	xennet_set_rx_rsp_cons(queue, cons);
	return err;
}