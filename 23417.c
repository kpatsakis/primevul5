static void gem_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = gem_realize;
    device_class_set_props(dc, gem_properties);
    dc->vmsd = &vmstate_cadence_gem;
    dc->reset = gem_reset;
}