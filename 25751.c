static bool max_queues_gt_1(void *opaque, int version_id)
{
    return VIRTIO_NET(opaque)->max_queues > 1;
}