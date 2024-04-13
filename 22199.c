static void sungem_do_tx_csum(SunGEMState *s)
{
    uint16_t start, off;
    uint32_t csum;

    start = (s->tx_first_ctl & TXDCTRL_CSTART) >> 15;
    off = (s->tx_first_ctl & TXDCTRL_COFF) >> 21;

    trace_sungem_tx_checksum(start, off);

    if (start > (s->tx_size - 2) || off > (s->tx_size - 2)) {
        trace_sungem_tx_checksum_oob();
        return;
    }

    csum = net_raw_checksum(s->tx_data + start, s->tx_size - start);
    stw_be_p(s->tx_data + off, csum);
}