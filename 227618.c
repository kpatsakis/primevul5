static void msf2_emac_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = msf2_emac_realize;
    dc->reset = msf2_emac_reset;
    dc->vmsd = &vmstate_msf2_emac;
    device_class_set_props(dc, msf2_emac_properties);
}