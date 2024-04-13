static void gem_set_link(NetClientState *nc)
{
    CadenceGEMState *s = qemu_get_nic_opaque(nc);

    DB_PRINT("\n");
    phy_update_link(s);
    gem_update_int_status(s);
}