static void sungem_reset_all(SunGEMState *s, bool pci_reset)
{
    trace_sungem_reset(pci_reset);

    sungem_reset_rx(s);
    sungem_reset_tx(s);

    s->gregs[GREG_IMASK >> 2] = 0xFFFFFFF;
    s->gregs[GREG_STAT >> 2] = 0;
    if (pci_reset) {
        uint8_t *ma = s->conf.macaddr.a;

        s->gregs[GREG_SWRST >> 2] = 0;
        s->macregs[MAC_ADDR0 >> 2] = (ma[4] << 8) | ma[5];
        s->macregs[MAC_ADDR1 >> 2] = (ma[2] << 8) | ma[3];
        s->macregs[MAC_ADDR2 >> 2] = (ma[0] << 8) | ma[1];
    } else {
        s->gregs[GREG_SWRST >> 2] &= GREG_SWRST_RSTOUT;
    }
    s->mifregs[MIF_CFG >> 2] = MIF_CFG_MDI0;
}