static void emac_set_link(NetClientState *nc)
{
    MSF2EmacState *s = qemu_get_nic_opaque(nc);

    msf2_phy_update_link(s);
}