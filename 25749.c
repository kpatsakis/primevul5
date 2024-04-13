static void virtio_net_set_multiqueue(VirtIONet *n, int multiqueue)
{
    int max = multiqueue ? n->max_queues : 1;

    n->multiqueue = multiqueue;
    virtio_net_change_num_queues(n, max);

    virtio_net_set_queues(n);
}