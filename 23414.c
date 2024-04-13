static void gem_reset(DeviceState *d)
{
    int i;
    CadenceGEMState *s = CADENCE_GEM(d);
    const uint8_t *a;
    uint32_t queues_mask = 0;

    DB_PRINT("\n");

    /* Set post reset register values */
    memset(&s->regs[0], 0, sizeof(s->regs));
    s->regs[GEM_NWCFG] = 0x00080000;
    s->regs[GEM_NWSTATUS] = 0x00000006;
    s->regs[GEM_DMACFG] = 0x00020784;
    s->regs[GEM_IMR] = 0x07ffffff;
    s->regs[GEM_TXPAUSE] = 0x0000ffff;
    s->regs[GEM_TXPARTIALSF] = 0x000003ff;
    s->regs[GEM_RXPARTIALSF] = 0x000003ff;
    s->regs[GEM_MODID] = s->revision;
    s->regs[GEM_DESCONF] = 0x02D00111;
    s->regs[GEM_DESCONF2] = 0x2ab10000 | s->jumbo_max_len;
    s->regs[GEM_DESCONF5] = 0x002f2045;
    s->regs[GEM_DESCONF6] = GEM_DESCONF6_64B_MASK;
    s->regs[GEM_INT_Q1_MASK] = 0x00000CE6;
    s->regs[GEM_JUMBO_MAX_LEN] = s->jumbo_max_len;

    if (s->num_priority_queues > 1) {
        queues_mask = MAKE_64BIT_MASK(1, s->num_priority_queues - 1);
        s->regs[GEM_DESCONF6] |= queues_mask;
    }

    /* Set MAC address */
    a = &s->conf.macaddr.a[0];
    s->regs[GEM_SPADDR1LO] = a[0] | (a[1] << 8) | (a[2] << 16) | (a[3] << 24);
    s->regs[GEM_SPADDR1HI] = a[4] | (a[5] << 8);

    for (i = 0; i < 4; i++) {
        s->sar_active[i] = false;
    }

    gem_phy_reset(s);

    gem_update_int_status(s);
}