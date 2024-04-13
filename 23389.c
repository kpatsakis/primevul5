static void gem_init_register_masks(CadenceGEMState *s)
{
    unsigned int i;
    /* Mask of register bits which are read only */
    memset(&s->regs_ro[0], 0, sizeof(s->regs_ro));
    s->regs_ro[GEM_NWCTRL]   = 0xFFF80000;
    s->regs_ro[GEM_NWSTATUS] = 0xFFFFFFFF;
    s->regs_ro[GEM_DMACFG]   = 0x8E00F000;
    s->regs_ro[GEM_TXSTATUS] = 0xFFFFFE08;
    s->regs_ro[GEM_RXQBASE]  = 0x00000003;
    s->regs_ro[GEM_TXQBASE]  = 0x00000003;
    s->regs_ro[GEM_RXSTATUS] = 0xFFFFFFF0;
    s->regs_ro[GEM_ISR]      = 0xFFFFFFFF;
    s->regs_ro[GEM_IMR]      = 0xFFFFFFFF;
    s->regs_ro[GEM_MODID]    = 0xFFFFFFFF;
    for (i = 0; i < s->num_priority_queues; i++) {
        s->regs_ro[GEM_INT_Q1_STATUS + i] = 0xFFFFFFFF;
        s->regs_ro[GEM_INT_Q1_ENABLE + i] = 0xFFFFF319;
        s->regs_ro[GEM_INT_Q1_DISABLE + i] = 0xFFFFF319;
        s->regs_ro[GEM_INT_Q1_MASK + i] = 0xFFFFFFFF;
    }

    /* Mask of register bits which are clear on read */
    memset(&s->regs_rtc[0], 0, sizeof(s->regs_rtc));
    s->regs_rtc[GEM_ISR]      = 0xFFFFFFFF;
    for (i = 0; i < s->num_priority_queues; i++) {
        s->regs_rtc[GEM_INT_Q1_STATUS + i] = 0x00000CE6;
    }

    /* Mask of register bits which are write 1 to clear */
    memset(&s->regs_w1c[0], 0, sizeof(s->regs_w1c));
    s->regs_w1c[GEM_TXSTATUS] = 0x000001F7;
    s->regs_w1c[GEM_RXSTATUS] = 0x0000000F;

    /* Mask of register bits which are write only */
    memset(&s->regs_wo[0], 0, sizeof(s->regs_wo));
    s->regs_wo[GEM_NWCTRL]   = 0x00073E60;
    s->regs_wo[GEM_IER]      = 0x07FFFFFF;
    s->regs_wo[GEM_IDR]      = 0x07FFFFFF;
    for (i = 0; i < s->num_priority_queues; i++) {
        s->regs_wo[GEM_INT_Q1_ENABLE + i] = 0x00000CE6;
        s->regs_wo[GEM_INT_Q1_DISABLE + i] = 0x00000CE6;
    }
}