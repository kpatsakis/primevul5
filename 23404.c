static ssize_t gem_receive(NetClientState *nc, const uint8_t *buf, size_t size)
{
    CadenceGEMState *s = qemu_get_nic_opaque(nc);
    unsigned   rxbufsize, bytes_to_copy;
    unsigned   rxbuf_offset;
    uint8_t   *rxbuf_ptr;
    bool first_desc = true;
    int maf;
    int q = 0;

    /* Is this destination MAC address "for us" ? */
    maf = gem_mac_address_filter(s, buf);
    if (maf == GEM_RX_REJECT) {
        return size;  /* no, drop siliently b/c it's not an error */
    }

    /* Discard packets with receive length error enabled ? */
    if (s->regs[GEM_NWCFG] & GEM_NWCFG_LERR_DISC) {
        unsigned type_len;

        /* Fish the ethertype / length field out of the RX packet */
        type_len = buf[12] << 8 | buf[13];
        /* It is a length field, not an ethertype */
        if (type_len < 0x600) {
            if (size < type_len) {
                /* discard */
                return -1;
            }
        }
    }

    /*
     * Determine configured receive buffer offset (probably 0)
     */
    rxbuf_offset = (s->regs[GEM_NWCFG] & GEM_NWCFG_BUFF_OFST_M) >>
                   GEM_NWCFG_BUFF_OFST_S;

    /* The configure size of each receive buffer.  Determines how many
     * buffers needed to hold this packet.
     */
    rxbufsize = ((s->regs[GEM_DMACFG] & GEM_DMACFG_RBUFSZ_M) >>
                 GEM_DMACFG_RBUFSZ_S) * GEM_DMACFG_RBUFSZ_MUL;
    bytes_to_copy = size;

    /* Hardware allows a zero value here but warns against it. To avoid QEMU
     * indefinite loops we enforce a minimum value here
     */
    if (rxbufsize < GEM_DMACFG_RBUFSZ_MUL) {
        rxbufsize = GEM_DMACFG_RBUFSZ_MUL;
    }

    /* Pad to minimum length. Assume FCS field is stripped, logic
     * below will increment it to the real minimum of 64 when
     * not FCS stripping
     */
    if (size < 60) {
        size = 60;
    }

    /* Strip of FCS field ? (usually yes) */
    if (s->regs[GEM_NWCFG] & GEM_NWCFG_STRIP_FCS) {
        rxbuf_ptr = (void *)buf;
    } else {
        unsigned crc_val;

        if (size > MAX_FRAME_SIZE - sizeof(crc_val)) {
            size = MAX_FRAME_SIZE - sizeof(crc_val);
        }
        bytes_to_copy = size;
        /* The application wants the FCS field, which QEMU does not provide.
         * We must try and calculate one.
         */

        memcpy(s->rx_packet, buf, size);
        memset(s->rx_packet + size, 0, MAX_FRAME_SIZE - size);
        rxbuf_ptr = s->rx_packet;
        crc_val = cpu_to_le32(crc32(0, s->rx_packet, MAX(size, 60)));
        memcpy(s->rx_packet + size, &crc_val, sizeof(crc_val));

        bytes_to_copy += 4;
        size += 4;
    }

    DB_PRINT("config bufsize: %u packet size: %zd\n", rxbufsize, size);

    /* Find which queue we are targeting */
    q = get_queue_from_screen(s, rxbuf_ptr, rxbufsize);

    if (size > gem_get_max_buf_len(s, false)) {
        qemu_log_mask(LOG_GUEST_ERROR, "rx frame too long\n");
        gem_set_isr(s, q, GEM_INT_AMBA_ERR);
        return -1;
    }

    while (bytes_to_copy) {
        hwaddr desc_addr;

        /* Do nothing if receive is not enabled. */
        if (!gem_can_receive(nc)) {
            return -1;
        }

        DB_PRINT("copy %" PRIu32 " bytes to 0x%" PRIx64 "\n",
                MIN(bytes_to_copy, rxbufsize),
                rx_desc_get_buffer(s, s->rx_desc[q]));

        /* Copy packet data to emulated DMA buffer */
        address_space_write(&s->dma_as, rx_desc_get_buffer(s, s->rx_desc[q]) +
                                                                  rxbuf_offset,
                            MEMTXATTRS_UNSPECIFIED, rxbuf_ptr,
                            MIN(bytes_to_copy, rxbufsize));
        rxbuf_ptr += MIN(bytes_to_copy, rxbufsize);
        bytes_to_copy -= MIN(bytes_to_copy, rxbufsize);

        rx_desc_clear_control(s->rx_desc[q]);

        /* Update the descriptor.  */
        if (first_desc) {
            rx_desc_set_sof(s->rx_desc[q]);
            first_desc = false;
        }
        if (bytes_to_copy == 0) {
            rx_desc_set_eof(s->rx_desc[q]);
            rx_desc_set_length(s->rx_desc[q], size);
        }
        rx_desc_set_ownership(s->rx_desc[q]);

        switch (maf) {
        case GEM_RX_PROMISCUOUS_ACCEPT:
            break;
        case GEM_RX_BROADCAST_ACCEPT:
            rx_desc_set_broadcast(s->rx_desc[q]);
            break;
        case GEM_RX_UNICAST_HASH_ACCEPT:
            rx_desc_set_unicast_hash(s->rx_desc[q]);
            break;
        case GEM_RX_MULTICAST_HASH_ACCEPT:
            rx_desc_set_multicast_hash(s->rx_desc[q]);
            break;
        case GEM_RX_REJECT:
            abort();
        default: /* SAR */
            rx_desc_set_sar(s->rx_desc[q], maf);
        }

        /* Descriptor write-back.  */
        desc_addr = gem_get_rx_desc_addr(s, q);
        address_space_write(&s->dma_as, desc_addr, MEMTXATTRS_UNSPECIFIED,
                            s->rx_desc[q],
                            sizeof(uint32_t) * gem_get_desc_len(s, true));

        /* Next descriptor */
        if (rx_desc_get_wrap(s->rx_desc[q])) {
            DB_PRINT("wrapping RX descriptor list\n");
            s->rx_desc_addr[q] = gem_get_rx_queue_base_addr(s, q);
        } else {
            DB_PRINT("incrementing RX descriptor list\n");
            s->rx_desc_addr[q] += 4 * gem_get_desc_len(s, true);
        }

        gem_get_rx_desc(s, q);
    }

    /* Count it */
    gem_receive_updatestats(s, buf, size);

    s->regs[GEM_RXSTATUS] |= GEM_RXSTATUS_FRMRCVD;
    gem_set_isr(s, q, GEM_INT_RXCMPL);

    /* Handle interrupt consequences */
    gem_update_int_status(s);

    return size;
}