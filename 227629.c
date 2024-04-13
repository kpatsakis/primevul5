static bool addr_filter_ok(MSF2EmacState *s, const uint8_t *buf)
{
    /* The broadcast MAC address: FF:FF:FF:FF:FF:FF */
    const uint8_t broadcast_addr[] = { 0xFF, 0xFF, 0xFF, 0xFF,
                                              0xFF, 0xFF };
    bool bcast_en = true;
    bool mcast_en = true;

    if (s->regs[R_FIFO_CFG5] & R_FIFO_CFG5_BCAST_MASK) {
        bcast_en = true; /* Broadcast dont care for drop circuitry */
    } else if (s->regs[R_FIFO_CFG4] & R_FIFO_CFG4_BCAST_MASK) {
        bcast_en = false;
    }

    if (s->regs[R_FIFO_CFG5] & R_FIFO_CFG5_MCAST_MASK) {
        mcast_en = true; /* Multicast dont care for drop circuitry */
    } else if (s->regs[R_FIFO_CFG4] & R_FIFO_CFG4_MCAST_MASK) {
        mcast_en = false;
    }

    if (!memcmp(buf, broadcast_addr, sizeof(broadcast_addr))) {
        return bcast_en;
    }

    if (buf[0] & 1) {
        return mcast_en;
    }

    return !memcmp(buf, s->mac_addr, sizeof(s->mac_addr));
}