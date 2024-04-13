static int sungem_check_rx_mac(SunGEMState *s, const uint8_t *mac, uint32_t crc)
{
    uint32_t rxcfg = s->macregs[MAC_RXCFG >> 2];
    uint32_t mac0, mac1, mac2;

    /* Promisc enabled ? */
    if (rxcfg & MAC_RXCFG_PROM) {
        return rx_match_promisc;
    }

    /* Format MAC address into dwords */
    mac0 = (mac[4] << 8) | mac[5];
    mac1 = (mac[2] << 8) | mac[3];
    mac2 = (mac[0] << 8) | mac[1];

    trace_sungem_rx_mac_check(mac0, mac1, mac2);

    /* Is this a broadcast frame ? */
    if (mac0 == 0xffff && mac1 == 0xffff && mac2 == 0xffff) {
        return rx_match_bcast;
    }

    /* TODO: Implement address filter registers (or we don't care ?) */

    /* Is this a multicast frame ? */
    if (mac[0] & 1) {
        trace_sungem_rx_mac_multicast();

        /* Promisc group enabled ? */
        if (rxcfg & MAC_RXCFG_PGRP) {
            return rx_match_allmcast;
        }

        /* TODO: Check MAC control frames (or we don't care) ? */

        /* Check hash filter (somebody check that's correct ?) */
        if (rxcfg & MAC_RXCFG_HFE) {
            uint32_t hash, idx;

            crc >>= 24;
            idx = (crc >> 2) & 0x3c;
            hash = s->macregs[(MAC_HASH0 + idx) >> 2];
            if (hash & (1 << (15 - (crc & 0xf)))) {
                return rx_match_mcast;
            }
        }
        return rx_no_match;
    }

    /* Main MAC check */
    trace_sungem_rx_mac_compare(s->macregs[MAC_ADDR0 >> 2],
                                s->macregs[MAC_ADDR1 >> 2],
                                s->macregs[MAC_ADDR2 >> 2]);

    if (mac0 == s->macregs[MAC_ADDR0 >> 2] &&
        mac1 == s->macregs[MAC_ADDR1 >> 2] &&
        mac2 == s->macregs[MAC_ADDR2 >> 2]) {
        return rx_match_mac;
    }

    /* Alt MAC check */
    if (mac0 == s->macregs[MAC_ADDR3 >> 2] &&
        mac1 == s->macregs[MAC_ADDR4 >> 2] &&
        mac2 == s->macregs[MAC_ADDR5 >> 2]) {
        return rx_match_altmac;
    }

    return rx_no_match;
}