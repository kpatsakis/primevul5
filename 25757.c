static void virtio_net_set_mrg_rx_bufs(VirtIONet *n, int mergeable_rx_bufs,
                                       int version_1, int hash_report)
{
    int i;
    NetClientState *nc;

    n->mergeable_rx_bufs = mergeable_rx_bufs;

    if (version_1) {
        n->guest_hdr_len = hash_report ?
            sizeof(struct virtio_net_hdr_v1_hash) :
            sizeof(struct virtio_net_hdr_mrg_rxbuf);
        n->rss_data.populate_hash = !!hash_report;
    } else {
        n->guest_hdr_len = n->mergeable_rx_bufs ?
            sizeof(struct virtio_net_hdr_mrg_rxbuf) :
            sizeof(struct virtio_net_hdr);
    }

    for (i = 0; i < n->max_queues; i++) {
        nc = qemu_get_subqueue(n->nic, i);

        if (peer_has_vnet_hdr(n) &&
            qemu_has_vnet_hdr_len(nc->peer, n->guest_hdr_len)) {
            qemu_set_vnet_hdr_len(nc->peer, n->guest_hdr_len);
            n->host_hdr_len = n->guest_hdr_len;
        }
    }
}