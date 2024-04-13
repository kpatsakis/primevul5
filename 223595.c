static void sm501_sysbus_init(Object *o)
{
    SM501SysBusState *sm501 = SYSBUS_SM501(o);
    SerialMM *smm = &sm501->serial;

    sysbus_init_child_obj(o, "serial", smm, sizeof(SerialMM), TYPE_SERIAL_MM);
    qdev_set_legacy_instance_id(DEVICE(smm), SM501_UART0, 2);
    qdev_prop_set_uint8(DEVICE(smm), "regshift", 2);
    qdev_prop_set_uint8(DEVICE(smm), "endianness", DEVICE_LITTLE_ENDIAN);

    object_property_add_alias(o, "chardev",
                              OBJECT(smm), "chardev");
}