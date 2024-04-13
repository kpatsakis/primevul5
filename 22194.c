static void sungem_eval_cascade_irq(SunGEMState *s)
{
    uint32_t stat, mask;

    mask = s->macregs[MAC_TXSTAT >> 2];
    stat = s->macregs[MAC_TXMASK >> 2];
    if (stat & ~mask) {
        sungem_update_status(s, GREG_STAT_TXMAC, true);
    } else {
        sungem_update_status(s, GREG_STAT_TXMAC, false);
    }

    mask = s->macregs[MAC_RXSTAT >> 2];
    stat = s->macregs[MAC_RXMASK >> 2];
    if (stat & ~mask) {
        sungem_update_status(s, GREG_STAT_RXMAC, true);
    } else {
        sungem_update_status(s, GREG_STAT_RXMAC, false);
    }

    mask = s->macregs[MAC_CSTAT >> 2];
    stat = s->macregs[MAC_MCMASK >> 2] & ~MAC_CSTAT_PTR;
    if (stat & ~mask) {
        sungem_update_status(s, GREG_STAT_MAC, true);
    } else {
        sungem_update_status(s, GREG_STAT_MAC, false);
    }
}