static ssize_t sungem_receive(NetClientState *nc, const uint8_t *buf,
                              size_t size)
{
    SunGEMState *s = qemu_get_nic_opaque(nc);
    PCIDevice *d = PCI_DEVICE(s);
    uint32_t mac_crc, done, kick, max_fsize;
    uint32_t fcs_size, ints, rxdma_cfg, rxmac_cfg, csum, coff;
    uint8_t smallbuf[60];
    struct gem_rxd desc;
    uint64_t dbase, baddr;
    unsigned int rx_cond;

    trace_sungem_rx_packet(size);

    rxmac_cfg = s->macregs[MAC_RXCFG >> 2];
    rxdma_cfg = s->rxdmaregs[RXDMA_CFG >> 2];
    max_fsize = s->macregs[MAC_MAXFSZ >> 2] & 0x7fff;

    /* If MAC or DMA disabled, can't receive */
    if (!(rxdma_cfg & RXDMA_CFG_ENABLE) ||
        !(rxmac_cfg & MAC_RXCFG_ENAB)) {
        trace_sungem_rx_disabled();
        return 0;
    }

    /* Size adjustment for FCS */
    if (rxmac_cfg & MAC_RXCFG_SFCS) {
        fcs_size = 0;
    } else {
        fcs_size = 4;
    }

    /* Discard frame smaller than a MAC or larger than max frame size
     * (when accounting for FCS)
     */
    if (size < 6 || (size + 4) > max_fsize) {
        trace_sungem_rx_bad_frame_size(size);
        /* XXX Increment error statistics ? */
        return size;
    }

    /* We don't drop too small frames since we get them in qemu, we pad
     * them instead. We should probably use the min frame size register
     * but I don't want to use a variable size staging buffer and I
     * know both MacOS and Linux use the default 64 anyway. We use 60
     * here to account for the non-existent FCS.
     */
    if (size < 60) {
        memcpy(smallbuf, buf, size);
        memset(&smallbuf[size], 0, 60 - size);
        buf = smallbuf;
        size = 60;
    }

    /* Get MAC crc */
    mac_crc = net_crc32_le(buf, ETH_ALEN);

    /* Packet isn't for me ? */
    rx_cond = sungem_check_rx_mac(s, buf, mac_crc);
    if (rx_cond == rx_no_match) {
        /* Just drop it */
        trace_sungem_rx_unmatched();
        return size;
    }

    /* Get ring pointers */
    kick = s->rxdmaregs[RXDMA_KICK >> 2] & s->rx_mask;
    done = s->rxdmaregs[RXDMA_DONE >> 2] & s->rx_mask;

    trace_sungem_rx_process(done, kick, s->rx_mask + 1);

    /* Ring full ? Can't receive */
    if (sungem_rx_full(s, kick, done)) {
        trace_sungem_rx_ringfull();
        return 0;
    }

    /* Note: The real GEM will fetch descriptors in blocks of 4,
     * for now we handle them one at a time, I think the driver will
     * cope
     */

    dbase = s->rxdmaregs[RXDMA_DBHI >> 2];
    dbase = (dbase << 32) | s->rxdmaregs[RXDMA_DBLOW >> 2];

    /* Read the next descriptor */
    pci_dma_read(d, dbase + done * sizeof(desc), &desc, sizeof(desc));

    trace_sungem_rx_desc(le64_to_cpu(desc.status_word),
                         le64_to_cpu(desc.buffer));

    /* Effective buffer address */
    baddr = le64_to_cpu(desc.buffer) & ~7ull;
    baddr |= (rxdma_cfg & RXDMA_CFG_FBOFF) >> 10;

    /* Write buffer out */
    pci_dma_write(d, baddr, buf, size);

    if (fcs_size) {
        /* Should we add an FCS ? Linux doesn't ask us to strip it,
         * however I believe nothing checks it... For now we just
         * do nothing. It's faster this way.
         */
    }

    /* Calculate the checksum */
    coff = (rxdma_cfg & RXDMA_CFG_CSUMOFF) >> 13;
    csum = net_raw_checksum((uint8_t *)buf + coff, size - coff);

    /* Build the updated descriptor */
    desc.status_word = (size + fcs_size) << 16;
    desc.status_word |= ((uint64_t)(mac_crc >> 16)) << 44;
    desc.status_word |= csum;
    if (rx_cond == rx_match_mcast) {
        desc.status_word |= RXDCTRL_HPASS;
    }
    if (rx_cond == rx_match_altmac) {
        desc.status_word |= RXDCTRL_ALTMAC;
    }
    desc.status_word = cpu_to_le64(desc.status_word);

    pci_dma_write(d, dbase + done * sizeof(desc), &desc, sizeof(desc));

    done = (done + 1) & s->rx_mask;
    s->rxdmaregs[RXDMA_DONE >> 2] = done;

    /* XXX Unconditionally set RX interrupt for now. The interrupt
     * mitigation timer might well end up adding more overhead than
     * helping here...
     */
    ints = GREG_STAT_RXDONE;
    if (sungem_rx_full(s, kick, done)) {
        ints |= GREG_STAT_RXNOBUF;
    }
    sungem_update_status(s, ints, true);

    return size;
}