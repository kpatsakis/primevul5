static void sungem_reset(DeviceState *dev)
{
    SunGEMState *s = SUNGEM(dev);

    sungem_reset_all(s, true);
}