static uint16_t virtio_net_handle_rss(VirtIONet *n,
                                      struct iovec *iov,
                                      unsigned int iov_cnt,
                                      bool do_rss)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    struct virtio_net_rss_config cfg;
    size_t s, offset = 0, size_get;
    uint16_t queues, i;
    struct {
        uint16_t us;
        uint8_t b;
    } QEMU_PACKED temp;
    const char *err_msg = "";
    uint32_t err_value = 0;

    if (do_rss && !virtio_vdev_has_feature(vdev, VIRTIO_NET_F_RSS)) {
        err_msg = "RSS is not negotiated";
        goto error;
    }
    if (!do_rss && !virtio_vdev_has_feature(vdev, VIRTIO_NET_F_HASH_REPORT)) {
        err_msg = "Hash report is not negotiated";
        goto error;
    }
    size_get = offsetof(struct virtio_net_rss_config, indirection_table);
    s = iov_to_buf(iov, iov_cnt, offset, &cfg, size_get);
    if (s != size_get) {
        err_msg = "Short command buffer";
        err_value = (uint32_t)s;
        goto error;
    }
    n->rss_data.hash_types = virtio_ldl_p(vdev, &cfg.hash_types);
    n->rss_data.indirections_len =
        virtio_lduw_p(vdev, &cfg.indirection_table_mask);
    n->rss_data.indirections_len++;
    if (!do_rss) {
        n->rss_data.indirections_len = 1;
    }
    if (!is_power_of_2(n->rss_data.indirections_len)) {
        err_msg = "Invalid size of indirection table";
        err_value = n->rss_data.indirections_len;
        goto error;
    }
    if (n->rss_data.indirections_len > VIRTIO_NET_RSS_MAX_TABLE_LEN) {
        err_msg = "Too large indirection table";
        err_value = n->rss_data.indirections_len;
        goto error;
    }
    n->rss_data.default_queue = do_rss ?
        virtio_lduw_p(vdev, &cfg.unclassified_queue) : 0;
    if (n->rss_data.default_queue >= n->max_queues) {
        err_msg = "Invalid default queue";
        err_value = n->rss_data.default_queue;
        goto error;
    }
    offset += size_get;
    size_get = sizeof(uint16_t) * n->rss_data.indirections_len;
    g_free(n->rss_data.indirections_table);
    n->rss_data.indirections_table = g_malloc(size_get);
    if (!n->rss_data.indirections_table) {
        err_msg = "Can't allocate indirections table";
        err_value = n->rss_data.indirections_len;
        goto error;
    }
    s = iov_to_buf(iov, iov_cnt, offset,
                   n->rss_data.indirections_table, size_get);
    if (s != size_get) {
        err_msg = "Short indirection table buffer";
        err_value = (uint32_t)s;
        goto error;
    }
    for (i = 0; i < n->rss_data.indirections_len; ++i) {
        uint16_t val = n->rss_data.indirections_table[i];
        n->rss_data.indirections_table[i] = virtio_lduw_p(vdev, &val);
    }
    offset += size_get;
    size_get = sizeof(temp);
    s = iov_to_buf(iov, iov_cnt, offset, &temp, size_get);
    if (s != size_get) {
        err_msg = "Can't get queues";
        err_value = (uint32_t)s;
        goto error;
    }
    queues = do_rss ? virtio_lduw_p(vdev, &temp.us) : n->curr_queues;
    if (queues == 0 || queues > n->max_queues) {
        err_msg = "Invalid number of queues";
        err_value = queues;
        goto error;
    }
    if (temp.b > VIRTIO_NET_RSS_MAX_KEY_SIZE) {
        err_msg = "Invalid key size";
        err_value = temp.b;
        goto error;
    }
    if (!temp.b && n->rss_data.hash_types) {
        err_msg = "No key provided";
        err_value = 0;
        goto error;
    }
    if (!temp.b && !n->rss_data.hash_types) {
        virtio_net_disable_rss(n);
        return queues;
    }
    offset += size_get;
    size_get = temp.b;
    s = iov_to_buf(iov, iov_cnt, offset, n->rss_data.key, size_get);
    if (s != size_get) {
        err_msg = "Can get key buffer";
        err_value = (uint32_t)s;
        goto error;
    }
    n->rss_data.enabled = true;

    if (!n->rss_data.populate_hash) {
        if (!virtio_net_attach_epbf_rss(n)) {
            /* EBPF must be loaded for vhost */
            if (get_vhost_net(qemu_get_queue(n->nic)->peer)) {
                warn_report("Can't load eBPF RSS for vhost");
                goto error;
            }
            /* fallback to software RSS */
            warn_report("Can't load eBPF RSS - fallback to software RSS");
            n->rss_data.enabled_software_rss = true;
        }
    } else {
        /* use software RSS for hash populating */
        /* and detach eBPF if was loaded before */
        virtio_net_detach_epbf_rss(n);
        n->rss_data.enabled_software_rss = true;
    }

    trace_virtio_net_rss_enable(n->rss_data.hash_types,
                                n->rss_data.indirections_len,
                                temp.b);
    return queues;
error:
    trace_virtio_net_rss_error(err_msg, err_value);
    virtio_net_disable_rss(n);
    return 0;
}