static void msf2_emac_realize(DeviceState *dev, Error **errp)
{
    MSF2EmacState *s = MSS_EMAC(dev);

    if (!s->dma_mr) {
        error_setg(errp, "MSS_EMAC 'ahb-bus' link not set");
        return;
    }

    address_space_init(&s->dma_as, s->dma_mr, "emac-ahb");

    qemu_macaddr_default_if_unset(&s->conf.macaddr);
    s->nic = qemu_new_nic(&net_msf2_emac_info, &s->conf,
                          object_get_typename(OBJECT(dev)), dev->id, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);
}