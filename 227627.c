static void msf2_emac_reset(DeviceState *dev)
{
    MSF2EmacState *s = MSS_EMAC(dev);

    msf2_emac_do_reset(s);
}