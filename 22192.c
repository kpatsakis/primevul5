static void sungem_process_tx_desc(SunGEMState *s, struct gem_txd *desc)
{
    PCIDevice *d = PCI_DEVICE(s);
    uint32_t len;

    /* If it's a start of frame, discard anything we had in the
     * buffer and start again. This should be an error condition
     * if we had something ... for now we ignore it
     */
    if (desc->control_word & TXDCTRL_SOF) {
        if (s->tx_first_ctl) {
            trace_sungem_tx_unfinished();
        }
        s->tx_size = 0;
        s->tx_first_ctl = desc->control_word;
    }

    /* Grab data size */
    len = desc->control_word & TXDCTRL_BUFSZ;

    /* Clamp it to our max size */
    if ((s->tx_size + len) > MAX_PACKET_SIZE) {
        trace_sungem_tx_overflow();
        len = MAX_PACKET_SIZE - s->tx_size;
    }

    /* Read the data */
    pci_dma_read(d, desc->buffer, &s->tx_data[s->tx_size], len);
    s->tx_size += len;

    /* If end of frame, send packet */
    if (desc->control_word & TXDCTRL_EOF) {
        trace_sungem_tx_finished(s->tx_size);

        /* Handle csum */
        if (s->tx_first_ctl & TXDCTRL_CENAB) {
            sungem_do_tx_csum(s);
        }

        /* Send it */
        sungem_send_packet(s, s->tx_data, s->tx_size);

        /* No more pending packet */
        s->tx_size = 0;
        s->tx_first_ctl = 0;
    }
}