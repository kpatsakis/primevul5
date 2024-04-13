static bool virtio_net_can_receive(NetClientState *nc)
{
    VirtIONet *n = qemu_get_nic_opaque(nc);
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    VirtIONetQueue *q = virtio_net_get_subqueue(nc);

    if (!vdev->vm_running) {
        return false;
    }

    if (nc->queue_index >= n->curr_queues) {
        return false;
    }

    if (!virtio_queue_ready(q->rx_vq) ||
        !(vdev->status & VIRTIO_CONFIG_S_DRIVER_OK)) {
        return false;
    }

    return true;
}