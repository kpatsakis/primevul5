static bool gem_can_receive(NetClientState *nc)
{
    CadenceGEMState *s;
    int i;

    s = qemu_get_nic_opaque(nc);

    /* Do nothing if receive is not enabled. */
    if (!(s->regs[GEM_NWCTRL] & GEM_NWCTRL_RXENA)) {
        if (s->can_rx_state != 1) {
            s->can_rx_state = 1;
            DB_PRINT("can't receive - no enable\n");
        }
        return false;
    }

    for (i = 0; i < s->num_priority_queues; i++) {
        if (rx_desc_get_ownership(s->rx_desc[i]) != 1) {
            break;
        }
    };

    if (i == s->num_priority_queues) {
        if (s->can_rx_state != 2) {
            s->can_rx_state = 2;
            DB_PRINT("can't receive - all the buffer descriptors are busy\n");
        }
        return false;
    }

    if (s->can_rx_state != 0) {
        s->can_rx_state = 0;
        DB_PRINT("can receive\n");
    }
    return true;
}