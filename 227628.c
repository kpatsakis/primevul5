static void emac_write(void *opaque, hwaddr addr, uint64_t val64,
        unsigned int size)
{
    MSF2EmacState *s = opaque;
    uint32_t value = val64;
    uint32_t enreqbits;
    uint8_t pktcnt;

    addr >>= 2;
    switch (addr) {
    case R_DMA_TX_CTL:
        s->regs[addr] = value;
        if (value & R_DMA_TX_CTL_EN_MASK) {
            msf2_dma_tx(s);
        }
        break;
    case R_DMA_RX_CTL:
        s->regs[addr] = value;
        if (value & R_DMA_RX_CTL_EN_MASK) {
            s->rx_desc = s->regs[R_DMA_RX_DESC];
            qemu_flush_queued_packets(qemu_get_queue(s->nic));
        }
        break;
    case R_CFG1:
        s->regs[addr] = value;
        if (value & R_CFG1_RESET_MASK) {
            msf2_emac_do_reset(s);
        }
        break;
    case R_FIFO_CFG0:
       /*
        * For our implementation, turning on modules is instantaneous,
        * so the states requested via the *ENREQ bits appear in the
        * *ENRPLY bits immediately. Also the reset bits to reset PE-MCXMAC
        * module are not emulated here since it deals with start of frames,
        * inter-packet gap and control frames.
        */
        enreqbits = extract32(value, 8, 5);
        s->regs[addr] = deposit32(value, 16, 5, enreqbits);
        break;
    case R_DMA_TX_DESC:
        if (value & 0x3) {
            qemu_log_mask(LOG_GUEST_ERROR, "Tx Descriptor address should be"
                          " 32 bit aligned\n");
        }
        /* Ignore [1:0] bits */
        s->regs[addr] = value & ~3;
        break;
    case R_DMA_RX_DESC:
        if (value & 0x3) {
            qemu_log_mask(LOG_GUEST_ERROR, "Rx Descriptor address should be"
                          " 32 bit aligned\n");
        }
        /* Ignore [1:0] bits */
        s->regs[addr] = value & ~3;
        break;
    case R_DMA_TX_STATUS:
        if (value & R_DMA_TX_STATUS_UNDERRUN_MASK) {
            s->regs[addr] &= ~R_DMA_TX_STATUS_UNDERRUN_MASK;
        }
        if (value & R_DMA_TX_STATUS_PKT_SENT_MASK) {
            pktcnt = FIELD_EX32(s->regs[addr], DMA_TX_STATUS, PKTCNT);
            pktcnt--;
            s->regs[addr] = FIELD_DP32(s->regs[addr], DMA_TX_STATUS,
                                       PKTCNT, pktcnt);
            if (pktcnt == 0) {
                s->regs[addr] &= ~R_DMA_TX_STATUS_PKT_SENT_MASK;
            }
        }
        break;
    case R_DMA_RX_STATUS:
        if (value & R_DMA_RX_STATUS_OVERFLOW_MASK) {
            s->regs[addr] &= ~R_DMA_RX_STATUS_OVERFLOW_MASK;
        }
        if (value & R_DMA_RX_STATUS_PKT_RCVD_MASK) {
            pktcnt = FIELD_EX32(s->regs[addr], DMA_RX_STATUS, PKTCNT);
            pktcnt--;
            s->regs[addr] = FIELD_DP32(s->regs[addr], DMA_RX_STATUS,
                                       PKTCNT, pktcnt);
            if (pktcnt == 0) {
                s->regs[addr] &= ~R_DMA_RX_STATUS_PKT_RCVD_MASK;
            }
        }
        break;
    case R_DMA_IRQ:
        break;
    case R_MII_CMD:
        if (value & R_MII_CMD_READ_MASK) {
            s->regs[R_MII_STS] = read_from_phy(s);
        }
        break;
    case R_MII_CTL:
        s->regs[addr] = value;
        write_to_phy(s);
        break;
    case R_STA1:
        s->regs[addr] = value;
       /*
        * R_STA1 [31:24] : octet 1 of mac address
        * R_STA1 [23:16] : octet 2 of mac address
        * R_STA1 [15:8] : octet 3 of mac address
        * R_STA1 [7:0] : octet 4 of mac address
        */
        stl_be_p(s->mac_addr, value);
        break;
    case R_STA2:
        s->regs[addr] = value;
       /*
        * R_STA2 [31:24] : octet 5 of mac address
        * R_STA2 [23:16] : octet 6 of mac address
        */
        stw_be_p(s->mac_addr + 4, value >> 16);
        break;
    default:
        if (addr >= ARRAY_SIZE(s->regs)) {
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__,
                          addr * 4);
            return;
        }
        s->regs[addr] = value;
        break;
    }
    emac_update_irq(s);
}