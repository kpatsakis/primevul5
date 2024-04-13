static void sungem_tx_kick(SunGEMState *s)
{
    PCIDevice *d = PCI_DEVICE(s);
    uint32_t comp, kick;
    uint32_t txdma_cfg, txmac_cfg, ints;
    uint64_t dbase;

    trace_sungem_tx_kick();

    /* Check that both TX MAC and TX DMA are enabled. We don't
     * handle DMA-less direct FIFO operations (we don't emulate
     * the FIFO at all).
     *
     * A write to TXDMA_KICK while DMA isn't enabled can happen
     * when the driver is resetting the pointer.
     */
    txdma_cfg = s->txdmaregs[TXDMA_CFG >> 2];
    txmac_cfg = s->macregs[MAC_TXCFG >> 2];
    if (!(txdma_cfg & TXDMA_CFG_ENABLE) ||
        !(txmac_cfg & MAC_TXCFG_ENAB)) {
        trace_sungem_tx_disabled();
        return;
    }

    /* XXX Test min frame size register ? */
    /* XXX Test max frame size register ? */

    dbase = s->txdmaregs[TXDMA_DBHI >> 2];
    dbase = (dbase << 32) | s->txdmaregs[TXDMA_DBLOW >> 2];

    comp = s->txdmaregs[TXDMA_TXDONE >> 2] & s->tx_mask;
    kick = s->txdmaregs[TXDMA_KICK >> 2] & s->tx_mask;

    trace_sungem_tx_process(comp, kick, s->tx_mask + 1);

    /* This is rather primitive for now, we just send everything we
     * can in one go, like e1000. Ideally we should do the sending
     * from some kind of background task
     */
    while (comp != kick) {
        struct gem_txd desc;

        /* Read the next descriptor */
        pci_dma_read(d, dbase + comp * sizeof(desc), &desc, sizeof(desc));

        /* Byteswap descriptor */
        desc.control_word = le64_to_cpu(desc.control_word);
        desc.buffer = le64_to_cpu(desc.buffer);
        trace_sungem_tx_desc(comp, desc.control_word, desc.buffer);

        /* Send it for processing */
        sungem_process_tx_desc(s, &desc);

        /* Interrupt */
        ints = GREG_STAT_TXDONE;
        if (desc.control_word & TXDCTRL_INTME) {
            ints |= GREG_STAT_TXINTME;
        }
        sungem_update_status(s, ints, true);

        /* Next ! */
        comp = (comp + 1) & s->tx_mask;
        s->txdmaregs[TXDMA_TXDONE >> 2] = comp;
    }

    /* We sent everything, set status/irq bit */
    sungem_update_status(s, GREG_STAT_TXALL, true);
}