static void gem_transmit_updatestats(CadenceGEMState *s, const uint8_t *packet,
                                     unsigned bytes)
{
    uint64_t octets;

    /* Total octets (bytes) transmitted */
    octets = ((uint64_t)(s->regs[GEM_OCTTXLO]) << 32) |
             s->regs[GEM_OCTTXHI];
    octets += bytes;
    s->regs[GEM_OCTTXLO] = octets >> 32;
    s->regs[GEM_OCTTXHI] = octets;

    /* Error-free Frames transmitted */
    s->regs[GEM_TXCNT]++;

    /* Error-free Broadcast Frames counter */
    if (!memcmp(packet, broadcast_addr, 6)) {
        s->regs[GEM_TXBCNT]++;
    }

    /* Error-free Multicast Frames counter */
    if (packet[0] == 0x01) {
        s->regs[GEM_TXMCNT]++;
    }

    if (bytes <= 64) {
        s->regs[GEM_TX64CNT]++;
    } else if (bytes <= 127) {
        s->regs[GEM_TX65CNT]++;
    } else if (bytes <= 255) {
        s->regs[GEM_TX128CNT]++;
    } else if (bytes <= 511) {
        s->regs[GEM_TX256CNT]++;
    } else if (bytes <= 1023) {
        s->regs[GEM_TX512CNT]++;
    } else if (bytes <= 1518) {
        s->regs[GEM_TX1024CNT]++;
    } else {
        s->regs[GEM_TX1519CNT]++;
    }
}