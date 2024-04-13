static void sungem_uninit(PCIDevice *dev)
{
    SunGEMState *s = SUNGEM(dev);

    qemu_del_nic(s->nic);
}