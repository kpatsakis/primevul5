static ssize_t emac_rx(NetClientState *nc, const uint8_t *buf, size_t size)
{
    MSF2EmacState *s = qemu_get_nic_opaque(nc);
    EmacDesc d;
    uint8_t pktcnt;
    uint32_t status;

    if (size > (s->regs[R_MAX_FRAME_LENGTH] & 0xFFFF)) {
        return size;
    }
    if (!addr_filter_ok(s, buf)) {
        return size;
    }

    emac_load_desc(s, &d, s->rx_desc);

    if (d.pktsize & EMPTY_MASK) {
        address_space_write(&s->dma_as, d.pktaddr, MEMTXATTRS_UNSPECIFIED,
                            buf, size & PKT_SIZE);
        d.pktsize = size & PKT_SIZE;
        emac_store_desc(s, &d, s->rx_desc);
        /* update received packets count */
        status = s->regs[R_DMA_RX_STATUS];
        pktcnt = FIELD_EX32(status, DMA_RX_STATUS, PKTCNT);
        pktcnt++;
        s->regs[R_DMA_RX_STATUS] = FIELD_DP32(status, DMA_RX_STATUS,
                                              PKTCNT, pktcnt);
        s->regs[R_DMA_RX_STATUS] |= R_DMA_RX_STATUS_PKT_RCVD_MASK;
        s->rx_desc = d.next;
    } else {
        s->regs[R_DMA_RX_CTL] &= ~R_DMA_RX_CTL_EN_MASK;
        s->regs[R_DMA_RX_STATUS] |= R_DMA_RX_STATUS_OVERFLOW_MASK;
    }
    emac_update_irq(s);
    return size;
}