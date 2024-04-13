static void virtio_net_vnet_endian_status(VirtIONet *n, uint8_t status)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    int queues = n->multiqueue ? n->max_queues : 1;

    if (virtio_net_started(n, status)) {
        /* Before using the device, we tell the network backend about the
         * endianness to use when parsing vnet headers. If the backend
         * can't do it, we fallback onto fixing the headers in the core
         * virtio-net code.
         */
        n->needs_vnet_hdr_swap = virtio_net_set_vnet_endian(vdev, n->nic->ncs,
                                                            queues, true);
    } else if (virtio_net_started(n, vdev->status)) {
        /* After using the device, we need to reset the network backend to
         * the default (guest native endianness), otherwise the guest may
         * lose network connectivity if it is rebooted into a different
         * endianness.
         */
        virtio_net_set_vnet_endian(vdev, n->nic->ncs, queues, false);
    }
}