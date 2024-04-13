static void gem_realize(DeviceState *dev, Error **errp)
{
    CadenceGEMState *s = CADENCE_GEM(dev);
    int i;

    address_space_init(&s->dma_as,
                       s->dma_mr ? s->dma_mr : get_system_memory(), "dma");

    if (s->num_priority_queues == 0 ||
        s->num_priority_queues > MAX_PRIORITY_QUEUES) {
        error_setg(errp, "Invalid num-priority-queues value: %" PRIx8,
                   s->num_priority_queues);
        return;
    } else if (s->num_type1_screeners > MAX_TYPE1_SCREENERS) {
        error_setg(errp, "Invalid num-type1-screeners value: %" PRIx8,
                   s->num_type1_screeners);
        return;
    } else if (s->num_type2_screeners > MAX_TYPE2_SCREENERS) {
        error_setg(errp, "Invalid num-type2-screeners value: %" PRIx8,
                   s->num_type2_screeners);
        return;
    }

    for (i = 0; i < s->num_priority_queues; ++i) {
        sysbus_init_irq(SYS_BUS_DEVICE(dev), &s->irq[i]);
    }

    qemu_macaddr_default_if_unset(&s->conf.macaddr);

    s->nic = qemu_new_nic(&net_gem_info, &s->conf,
                          object_get_typename(OBJECT(dev)), dev->id, s);

    if (s->jumbo_max_len > MAX_FRAME_SIZE) {
        error_setg(errp, "jumbo-max-len is greater than %d",
                  MAX_FRAME_SIZE);
        return;
    }
}