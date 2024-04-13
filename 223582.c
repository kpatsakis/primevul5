static void sm501_reset_sysbus(DeviceState *dev)
{
    SM501SysBusState *s = SYSBUS_SM501(dev);
    sm501_reset(&s->state);
}