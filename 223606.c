static void sm501_sysbus_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = sm501_realize_sysbus;
    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->desc = "SM501 Multimedia Companion";
    device_class_set_props(dc, sm501_sysbus_properties);
    dc->reset = sm501_reset_sysbus;
    dc->vmsd = &vmstate_sm501_sysbus;
}