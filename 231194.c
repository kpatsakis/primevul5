static int vhost_net_buf_peek_len(void *ptr)
{
	if (tun_is_xdp_frame(ptr)) {
		struct xdp_frame *xdpf = tun_ptr_to_xdp(ptr);

		return xdpf->len;
	}

	return __skb_array_len_with_tag(ptr);
}