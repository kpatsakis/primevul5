static int virtio_net_tx_waiting_pre_save(void *opaque)
{
    struct VirtIONetMigTmp *tmp = opaque;

    tmp->vqs_1 = tmp->parent->vqs + 1;
    tmp->curr_queues_1 = tmp->parent->curr_queues - 1;
    if (tmp->parent->curr_queues == 0) {
        tmp->curr_queues_1 = 0;
    }

    return 0;
}