static void virtio_net_set_features(VirtIODevice *vdev, uint64_t features)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    Error *err = NULL;
    int i;

    if (n->mtu_bypass_backend &&
            !virtio_has_feature(vdev->backend_features, VIRTIO_NET_F_MTU)) {
        features &= ~(1ULL << VIRTIO_NET_F_MTU);
    }

    virtio_net_set_multiqueue(n,
                              virtio_has_feature(features, VIRTIO_NET_F_RSS) ||
                              virtio_has_feature(features, VIRTIO_NET_F_MQ));

    virtio_net_set_mrg_rx_bufs(n,
                               virtio_has_feature(features,
                                                  VIRTIO_NET_F_MRG_RXBUF),
                               virtio_has_feature(features,
                                                  VIRTIO_F_VERSION_1),
                               virtio_has_feature(features,
                                                  VIRTIO_NET_F_HASH_REPORT));

    n->rsc4_enabled = virtio_has_feature(features, VIRTIO_NET_F_RSC_EXT) &&
        virtio_has_feature(features, VIRTIO_NET_F_GUEST_TSO4);
    n->rsc6_enabled = virtio_has_feature(features, VIRTIO_NET_F_RSC_EXT) &&
        virtio_has_feature(features, VIRTIO_NET_F_GUEST_TSO6);
    n->rss_data.redirect = virtio_has_feature(features, VIRTIO_NET_F_RSS);

    if (n->has_vnet_hdr) {
        n->curr_guest_offloads =
            virtio_net_guest_offloads_by_features(features);
        virtio_net_apply_guest_offloads(n);
    }

    for (i = 0;  i < n->max_queues; i++) {
        NetClientState *nc = qemu_get_subqueue(n->nic, i);

        if (!get_vhost_net(nc->peer)) {
            continue;
        }
        vhost_net_ack_features(get_vhost_net(nc->peer), features);
    }

    if (virtio_has_feature(features, VIRTIO_NET_F_CTRL_VLAN)) {
        memset(n->vlans, 0, MAX_VLAN >> 3);
    } else {
        memset(n->vlans, 0xff, MAX_VLAN >> 3);
    }

    if (virtio_has_feature(features, VIRTIO_NET_F_STANDBY)) {
        qapi_event_send_failover_negotiated(n->netclient_name);
        qatomic_set(&n->failover_primary_hidden, false);
        failover_add_primary(n, &err);
        if (err) {
            warn_report_err(err);
        }
    }
}