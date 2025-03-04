static void mcf_fec_do_tx(mcf_fec_state *s)
{
    uint32_t addr;
    mcf_fec_bd bd;
    int frame_size;
    int len;
    uint8_t frame[FEC_MAX_FRAME_SIZE];
    uint8_t *ptr;

    DPRINTF("do_tx\n");
    ptr = frame;
    frame_size = 0;
    addr = s->tx_descriptor;
    while (1) {
        mcf_fec_read_bd(&bd, addr);
        DPRINTF("tx_bd %x flags %04x len %d data %08x\n",
                addr, bd.flags, bd.length, bd.data);
        if ((bd.flags & FEC_BD_R) == 0) {
            /* Run out of descriptors to transmit.  */
            break;
        }
        len = bd.length;
        if (frame_size + len > FEC_MAX_FRAME_SIZE) {
            len = FEC_MAX_FRAME_SIZE - frame_size;
            s->eir |= FEC_INT_BABT;
        }
        cpu_physical_memory_read(bd.data, ptr, len);
        ptr += len;
        frame_size += len;
        if (bd.flags & FEC_BD_L) {
            /* Last buffer in frame.  */
            DPRINTF("Sending packet\n");
            qemu_send_packet(qemu_get_queue(s->nic), frame, len);
            ptr = frame;
            frame_size = 0;
            s->eir |= FEC_INT_TXF;
        }
        s->eir |= FEC_INT_TXB;
        bd.flags &= ~FEC_BD_R;
        /* Write back the modified descriptor.  */
        mcf_fec_write_bd(&bd, addr);
        /* Advance to the next descriptor.  */
        if ((bd.flags & FEC_BD_W) != 0) {
            addr = s->etdsr;
        } else {
            addr += 8;
        }
    }
    s->tx_descriptor = addr;
}