static int virtio_net_tx_waiting_pre_load(void *opaque)
{
    struct VirtIONetMigTmp *tmp = opaque;

    /* Reuse the pointer setup from save */
    virtio_net_tx_waiting_pre_save(opaque);

    if (tmp->parent->curr_queues > tmp->parent->max_queues) {
        error_report("virtio-net: curr_queues %x > max_queues %x",
            tmp->parent->curr_queues, tmp->parent->max_queues);

        return -EINVAL;
    }

    return 0; /* all good */
}