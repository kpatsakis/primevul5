static bool sungem_can_receive(NetClientState *nc)
{
    SunGEMState *s = qemu_get_nic_opaque(nc);
    uint32_t kick, done, rxdma_cfg, rxmac_cfg;
    bool full;

    rxmac_cfg = s->macregs[MAC_RXCFG >> 2];
    rxdma_cfg = s->rxdmaregs[RXDMA_CFG >> 2];

    /* If MAC disabled, can't receive */
    if ((rxmac_cfg & MAC_RXCFG_ENAB) == 0) {
        trace_sungem_rx_mac_disabled();
        return false;
    }
    if ((rxdma_cfg & RXDMA_CFG_ENABLE) == 0) {
        trace_sungem_rx_txdma_disabled();
        return false;
    }

    /* Check RX availability */
    kick = s->rxdmaregs[RXDMA_KICK >> 2];
    done = s->rxdmaregs[RXDMA_DONE >> 2];
    full = sungem_rx_full(s, kick, done);

    trace_sungem_rx_check(!full, kick, done);

    return !full;
}