static void virtio_net_change_num_queues(VirtIONet *n, int new_max_queues)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    int old_num_queues = virtio_get_num_queues(vdev);
    int new_num_queues = new_max_queues * 2 + 1;
    int i;

    assert(old_num_queues >= 3);
    assert(old_num_queues % 2 == 1);

    if (old_num_queues == new_num_queues) {
        return;
    }

    /*
     * We always need to remove and add ctrl vq if
     * old_num_queues != new_num_queues. Remove ctrl_vq first,
     * and then we only enter one of the following two loops.
     */
    virtio_del_queue(vdev, old_num_queues - 1);

    for (i = new_num_queues - 1; i < old_num_queues - 1; i += 2) {
        /* new_num_queues < old_num_queues */
        virtio_net_del_queue(n, i / 2);
    }

    for (i = old_num_queues - 1; i < new_num_queues - 1; i += 2) {
        /* new_num_queues > old_num_queues */
        virtio_net_add_queue(n, i / 2);
    }

    /* add ctrl_vq last */
    n->ctrl_vq = virtio_add_queue(vdev, 64, virtio_net_handle_ctrl);
}