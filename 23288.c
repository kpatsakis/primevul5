static u32 xennet_run_xdp(struct netfront_queue *queue, struct page *pdata,
		   struct xen_netif_rx_response *rx, struct bpf_prog *prog,
		   struct xdp_buff *xdp, bool *need_xdp_flush)
{
	struct xdp_frame *xdpf;
	u32 len = rx->status;
	u32 act;
	int err;

	xdp_init_buff(xdp, XEN_PAGE_SIZE - XDP_PACKET_HEADROOM,
		      &queue->xdp_rxq);
	xdp_prepare_buff(xdp, page_address(pdata), XDP_PACKET_HEADROOM,
			 len, false);

	act = bpf_prog_run_xdp(prog, xdp);
	switch (act) {
	case XDP_TX:
		get_page(pdata);
		xdpf = xdp_convert_buff_to_frame(xdp);
		err = xennet_xdp_xmit(queue->info->netdev, 1, &xdpf, 0);
		if (unlikely(!err))
			xdp_return_frame_rx_napi(xdpf);
		else if (unlikely(err < 0))
			trace_xdp_exception(queue->info->netdev, prog, act);
		break;
	case XDP_REDIRECT:
		get_page(pdata);
		err = xdp_do_redirect(queue->info->netdev, xdp, prog);
		*need_xdp_flush = true;
		if (unlikely(err))
			trace_xdp_exception(queue->info->netdev, prog, act);
		break;
	case XDP_PASS:
	case XDP_DROP:
		break;

	case XDP_ABORTED:
		trace_xdp_exception(queue->info->netdev, prog, act);
		break;

	default:
		bpf_warn_invalid_xdp_action(queue->info->netdev, prog, act);
	}

	return act;
}