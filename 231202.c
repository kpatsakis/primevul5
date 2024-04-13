static int vhost_net_build_xdp(struct vhost_net_virtqueue *nvq,
			       struct iov_iter *from)
{
	struct vhost_virtqueue *vq = &nvq->vq;
	struct vhost_net *net = container_of(vq->dev, struct vhost_net,
					     dev);
	struct socket *sock = vq->private_data;
	struct page_frag *alloc_frag = &net->page_frag;
	struct virtio_net_hdr *gso;
	struct xdp_buff *xdp = &nvq->xdp[nvq->batched_xdp];
	struct tun_xdp_hdr *hdr;
	size_t len = iov_iter_count(from);
	int headroom = vhost_sock_xdp(sock) ? XDP_PACKET_HEADROOM : 0;
	int buflen = SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	int pad = SKB_DATA_ALIGN(VHOST_NET_RX_PAD + headroom + nvq->sock_hlen);
	int sock_hlen = nvq->sock_hlen;
	void *buf;
	int copied;

	if (unlikely(len < nvq->sock_hlen))
		return -EFAULT;

	if (SKB_DATA_ALIGN(len + pad) +
	    SKB_DATA_ALIGN(sizeof(struct skb_shared_info)) > PAGE_SIZE)
		return -ENOSPC;

	buflen += SKB_DATA_ALIGN(len + pad);
	alloc_frag->offset = ALIGN((u64)alloc_frag->offset, SMP_CACHE_BYTES);
	if (unlikely(!vhost_net_page_frag_refill(net, buflen,
						 alloc_frag, GFP_KERNEL)))
		return -ENOMEM;

	buf = (char *)page_address(alloc_frag->page) + alloc_frag->offset;
	copied = copy_page_from_iter(alloc_frag->page,
				     alloc_frag->offset +
				     offsetof(struct tun_xdp_hdr, gso),
				     sock_hlen, from);
	if (copied != sock_hlen)
		return -EFAULT;

	hdr = buf;
	gso = &hdr->gso;

	if ((gso->flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) &&
	    vhost16_to_cpu(vq, gso->csum_start) +
	    vhost16_to_cpu(vq, gso->csum_offset) + 2 >
	    vhost16_to_cpu(vq, gso->hdr_len)) {
		gso->hdr_len = cpu_to_vhost16(vq,
			       vhost16_to_cpu(vq, gso->csum_start) +
			       vhost16_to_cpu(vq, gso->csum_offset) + 2);

		if (vhost16_to_cpu(vq, gso->hdr_len) > len)
			return -EINVAL;
	}

	len -= sock_hlen;
	copied = copy_page_from_iter(alloc_frag->page,
				     alloc_frag->offset + pad,
				     len, from);
	if (copied != len)
		return -EFAULT;

	xdp->data_hard_start = buf;
	xdp->data = buf + pad;
	xdp->data_end = xdp->data + len;
	hdr->buflen = buflen;

	--net->refcnt_bias;
	alloc_frag->offset += buflen;

	++nvq->batched_xdp;

	return 0;
}