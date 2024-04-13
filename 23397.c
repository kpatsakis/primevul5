static uint32_t gem_get_queue_base_addr(CadenceGEMState *s, bool tx, int q)
{
    uint32_t base_addr = 0;

    switch (q) {
    case 0:
        base_addr = s->regs[tx ? GEM_TXQBASE : GEM_RXQBASE];
        break;
    case 1 ... (MAX_PRIORITY_QUEUES - 1):
        base_addr = s->regs[(tx ? GEM_TRANSMIT_Q1_PTR :
                                 GEM_RECEIVE_Q1_PTR) + q - 1];
        break;
    default:
        g_assert_not_reached();
    };

    return base_addr;
}