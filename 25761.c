static void virtio_net_reset(VirtIODevice *vdev)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    int i;

    /* Reset back to compatibility mode */
    n->promisc = 1;
    n->allmulti = 0;
    n->alluni = 0;
    n->nomulti = 0;
    n->nouni = 0;
    n->nobcast = 0;
    /* multiqueue is disabled by default */
    n->curr_queues = 1;
    timer_del(n->announce_timer.tm);
    n->announce_timer.round = 0;
    n->status &= ~VIRTIO_NET_S_ANNOUNCE;

    /* Flush any MAC and VLAN filter table state */
    n->mac_table.in_use = 0;
    n->mac_table.first_multi = 0;
    n->mac_table.multi_overflow = 0;
    n->mac_table.uni_overflow = 0;
    memset(n->mac_table.macs, 0, MAC_TABLE_ENTRIES * ETH_ALEN);
    memcpy(&n->mac[0], &n->nic->conf->macaddr, sizeof(n->mac));
    qemu_format_nic_info_str(qemu_get_queue(n->nic), n->mac);
    memset(n->vlans, 0, MAX_VLAN >> 3);

    /* Flush any async TX */
    for (i = 0;  i < n->max_queues; i++) {
        NetClientState *nc = qemu_get_subqueue(n->nic, i);

        if (nc->peer) {
            qemu_flush_or_purge_queued_packets(nc->peer, true);
            assert(!virtio_net_get_subqueue(nc)->async_tx.elem);
        }
    }
}