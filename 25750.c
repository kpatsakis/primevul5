static void virtio_net_set_queues(VirtIONet *n)
{
    int i;
    int r;

    if (n->nic->peer_deleted) {
        return;
    }

    for (i = 0; i < n->max_queues; i++) {
        if (i < n->curr_queues) {
            r = peer_attach(n, i);
            assert(!r);
        } else {
            r = peer_detach(n, i);
            assert(!r);
        }
    }
}