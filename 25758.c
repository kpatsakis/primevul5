static void virtio_net_get_config(VirtIODevice *vdev, uint8_t *config)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    struct virtio_net_config netcfg;
    NetClientState *nc = qemu_get_queue(n->nic);
    static const MACAddr zero = { .a = { 0, 0, 0, 0, 0, 0 } };

    int ret = 0;
    memset(&netcfg, 0 , sizeof(struct virtio_net_config));
    virtio_stw_p(vdev, &netcfg.status, n->status);
    virtio_stw_p(vdev, &netcfg.max_virtqueue_pairs, n->max_queues);
    virtio_stw_p(vdev, &netcfg.mtu, n->net_conf.mtu);
    memcpy(netcfg.mac, n->mac, ETH_ALEN);
    virtio_stl_p(vdev, &netcfg.speed, n->net_conf.speed);
    netcfg.duplex = n->net_conf.duplex;
    netcfg.rss_max_key_size = VIRTIO_NET_RSS_MAX_KEY_SIZE;
    virtio_stw_p(vdev, &netcfg.rss_max_indirection_table_length,
                 virtio_host_has_feature(vdev, VIRTIO_NET_F_RSS) ?
                 VIRTIO_NET_RSS_MAX_TABLE_LEN : 1);
    virtio_stl_p(vdev, &netcfg.supported_hash_types,
                 VIRTIO_NET_RSS_SUPPORTED_HASHES);
    memcpy(config, &netcfg, n->config_size);

    /*
     * Is this VDPA? No peer means not VDPA: there's no way to
     * disconnect/reconnect a VDPA peer.
     */
    if (nc->peer && nc->peer->info->type == NET_CLIENT_DRIVER_VHOST_VDPA) {
        ret = vhost_net_get_config(get_vhost_net(nc->peer), (uint8_t *)&netcfg,
                                   n->config_size);
        if (ret != -1) {
            /*
             * Some NIC/kernel combinations present 0 as the mac address.  As
             * that is not a legal address, try to proceed with the
             * address from the QEMU command line in the hope that the
             * address has been configured correctly elsewhere - just not
             * reported by the device.
             */
            if (memcmp(&netcfg.mac, &zero, sizeof(zero)) == 0) {
                info_report("Zero hardware mac address detected. Ignoring.");
                memcpy(netcfg.mac, n->mac, ETH_ALEN);
            }
            memcpy(config, &netcfg, n->config_size);
        }
    }
}