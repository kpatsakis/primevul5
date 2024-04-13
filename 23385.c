static int get_queue_from_screen(CadenceGEMState *s, uint8_t *rxbuf_ptr,
                                 unsigned rxbufsize)
{
    uint32_t reg;
    bool matched, mismatched;
    int i, j;

    for (i = 0; i < s->num_type1_screeners; i++) {
        reg = s->regs[GEM_SCREENING_TYPE1_REGISTER_0 + i];
        matched = false;
        mismatched = false;

        /* Screening is based on UDP Port */
        if (reg & GEM_ST1R_UDP_PORT_MATCH_ENABLE) {
            uint16_t udp_port = rxbuf_ptr[14 + 22] << 8 | rxbuf_ptr[14 + 23];
            if (udp_port == extract32(reg, GEM_ST1R_UDP_PORT_MATCH_SHIFT,
                                           GEM_ST1R_UDP_PORT_MATCH_WIDTH)) {
                matched = true;
            } else {
                mismatched = true;
            }
        }

        /* Screening is based on DS/TC */
        if (reg & GEM_ST1R_DSTC_ENABLE) {
            uint8_t dscp = rxbuf_ptr[14 + 1];
            if (dscp == extract32(reg, GEM_ST1R_DSTC_MATCH_SHIFT,
                                       GEM_ST1R_DSTC_MATCH_WIDTH)) {
                matched = true;
            } else {
                mismatched = true;
            }
        }

        if (matched && !mismatched) {
            return extract32(reg, GEM_ST1R_QUEUE_SHIFT, GEM_ST1R_QUEUE_WIDTH);
        }
    }

    for (i = 0; i < s->num_type2_screeners; i++) {
        reg = s->regs[GEM_SCREENING_TYPE2_REGISTER_0 + i];
        matched = false;
        mismatched = false;

        if (reg & GEM_ST2R_ETHERTYPE_ENABLE) {
            uint16_t type = rxbuf_ptr[12] << 8 | rxbuf_ptr[13];
            int et_idx = extract32(reg, GEM_ST2R_ETHERTYPE_INDEX_SHIFT,
                                        GEM_ST2R_ETHERTYPE_INDEX_WIDTH);

            if (et_idx > s->num_type2_screeners) {
                qemu_log_mask(LOG_GUEST_ERROR, "Out of range ethertype "
                              "register index: %d\n", et_idx);
            }
            if (type == s->regs[GEM_SCREENING_TYPE2_ETHERTYPE_REG_0 +
                                et_idx]) {
                matched = true;
            } else {
                mismatched = true;
            }
        }

        /* Compare A, B, C */
        for (j = 0; j < 3; j++) {
            uint32_t cr0, cr1, mask;
            uint16_t rx_cmp;
            int offset;
            int cr_idx = extract32(reg, GEM_ST2R_COMPARE_A_SHIFT + j * 6,
                                        GEM_ST2R_COMPARE_WIDTH);

            if (!(reg & (GEM_ST2R_COMPARE_A_ENABLE << (j * 6)))) {
                continue;
            }
            if (cr_idx > s->num_type2_screeners) {
                qemu_log_mask(LOG_GUEST_ERROR, "Out of range compare "
                              "register index: %d\n", cr_idx);
            }

            cr0 = s->regs[GEM_TYPE2_COMPARE_0_WORD_0 + cr_idx * 2];
            cr1 = s->regs[GEM_TYPE2_COMPARE_0_WORD_0 + cr_idx * 2 + 1];
            offset = extract32(cr1, GEM_T2CW1_OFFSET_VALUE_SHIFT,
                                    GEM_T2CW1_OFFSET_VALUE_WIDTH);

            switch (extract32(cr1, GEM_T2CW1_COMPARE_OFFSET_SHIFT,
                                   GEM_T2CW1_COMPARE_OFFSET_WIDTH)) {
            case 3: /* Skip UDP header */
                qemu_log_mask(LOG_UNIMP, "TCP compare offsets"
                              "unimplemented - assuming UDP\n");
                offset += 8;
                /* Fallthrough */
            case 2: /* skip the IP header */
                offset += 20;
                /* Fallthrough */
            case 1: /* Count from after the ethertype */
                offset += 14;
                break;
            case 0:
                /* Offset from start of frame */
                break;
            }

            rx_cmp = rxbuf_ptr[offset] << 8 | rxbuf_ptr[offset];
            mask = extract32(cr0, 0, 16);

            if ((rx_cmp & mask) == (extract32(cr0, 16, 16) & mask)) {
                matched = true;
            } else {
                mismatched = true;
            }
        }

        if (matched && !mismatched) {
            return extract32(reg, GEM_ST2R_QUEUE_SHIFT, GEM_ST2R_QUEUE_WIDTH);
        }
    }

    /* We made it here, assume it's queue 0 */
    return 0;
}