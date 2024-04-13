static int dissect_dvb_s2_gse(tvbuff_t *tvb, int cur_off, proto_tree *tree, packet_info *pinfo, int bytes_available)
{
    int         new_off                      = 0;
    int         frag_len;
    guint16     gse_hdr, data_len, padding_len, gse_proto = 0;

    proto_item *ti;
    proto_item *ttf;
    proto_tree *dvb_s2_gse_tree, *dvb_s2_gse_ncr_tree;

    tvbuff_t   *next_tvb, *data_tvb;
    gboolean   dissected = FALSE;
    gboolean   update_col_info = TRUE;

    static int * const gse_header_bitfields[] = {
        &hf_dvb_s2_gse_hdr_start,
        &hf_dvb_s2_gse_hdr_stop,
        &hf_dvb_s2_gse_hdr_labeltype,
        &hf_dvb_s2_gse_hdr_length,
        NULL
    };

    col_append_str(pinfo->cinfo, COL_INFO, " GSE");

    /* get the GSE header */
    gse_hdr = tvb_get_ntohs(tvb, cur_off + DVB_S2_GSE_OFFS_HDR);

    /* check if this is just padding, which takes up the rest of the frame */
    if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_START_POS) &&
        BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_STOP_POS) &&
        BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_LABELTYPE_POS1) && BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_LABELTYPE_POS2)) {

        padding_len = bytes_available;
        proto_tree_add_uint_format(tree, hf_dvb_s2_gse_padding, tvb, cur_off + new_off, padding_len, padding_len,
                                   "DVB-S2 GSE Padding, Length: %d", padding_len);
        col_append_str(pinfo->cinfo, COL_INFO, " pad");
        new_off += padding_len;

        return new_off;
    } else {
        /* Not padding, parse as a GSE Header */
        new_off += 2;
        frag_len = (gse_hdr & DVB_S2_GSE_HDR_LENGTH_MASK)+2;
        ti = proto_tree_add_item(tree, proto_dvb_s2_gse, tvb, cur_off, frag_len, ENC_NA);
        dvb_s2_gse_tree = proto_item_add_subtree(ti, ett_dvb_s2_gse);

        proto_tree_add_bitmask_with_flags(dvb_s2_gse_tree, tvb, cur_off + DVB_S2_GSE_OFFS_HDR, hf_dvb_s2_gse_hdr,
            ett_dvb_s2_gse_hdr, gse_header_bitfields, ENC_BIG_ENDIAN, BMT_NO_TFS);

        /* Get the fragment ID for reassembly */
        guint8 fragid = tvb_get_guint8(tvb, cur_off + new_off);
        if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_START_POS) || BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_STOP_POS)) {
            /* Not a start or end packet, add only the fragid */
            proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_fragid, tvb, cur_off + new_off, 1, ENC_BIG_ENDIAN);

            new_off += 1;
        }
        if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_START_POS) && BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_STOP_POS)) {
            /* Start packet, add the fragment size */
            proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_totlength, tvb, cur_off + new_off, 2, ENC_BIG_ENDIAN);
            col_append_str(pinfo->cinfo, COL_INFO, "(frag) ");

            new_off += 2;
        }
        if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_START_POS)) {
            /* Start packet, decode the header */
            gse_proto = tvb_get_ntohs(tvb, cur_off + new_off);

            /* Protocol Type */
            if (gse_proto <= 1535) {
                /* Type 1 (Next-Header Type field) */
                proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_proto_next_header, tvb, cur_off + new_off, 2, ENC_BIG_ENDIAN);
            }
            else {
                /* Type 2 (EtherType compatible Type Fields) */
                proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_proto_ethertype, tvb, cur_off + new_off, 2, ENC_BIG_ENDIAN);
            }
            new_off += 2;

            if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_LABELTYPE_POS1) && BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_LABELTYPE_POS2)) {
                /* 6 byte label */
                if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_STOP_POS))
                    col_append_str(pinfo->cinfo, COL_INFO, "6 ");

                proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_label6, tvb, cur_off + new_off, 6, ENC_NA);

                new_off += 6;
            } else if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_LABELTYPE_POS1) &&
                       BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_LABELTYPE_POS2)) {
                /* 3 byte label */
                if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_STOP_POS))
                    col_append_str(pinfo->cinfo, COL_INFO, "3 ");

                proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_label3, tvb, cur_off + new_off, 3, ENC_BIG_ENDIAN);

                new_off += 3;
            } else {
                /* 0 byte label */
                if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_STOP_POS))
                    col_append_str(pinfo->cinfo, COL_INFO, "0 ");
            }
            if (gse_proto < 0x0600 && gse_proto >= 0x100) {
                /* Only display optional extension headers */
                /* TODO: needs to be tested */

                /* TODO: implementation needs to be checked (len of ext-header??) */
                proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_exthdr, tvb, cur_off + new_off, 1, ENC_BIG_ENDIAN);

                new_off += 1;
            }
        }
        else
        {
            /* correct cinfo */
            col_append_str(pinfo->cinfo, COL_INFO, "(frag) ");
        }

        next_tvb = tvb_new_subset_remaining(tvb, cur_off + new_off);

        if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_START_POS) && BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_STOP_POS)) {
            data_len = (gse_hdr & DVB_S2_GSE_HDR_LENGTH_MASK) - (new_off - DVB_S2_GSE_MINSIZE) - DVB_S2_GSE_CRC32_LEN;
        } else {
            data_len = (gse_hdr & DVB_S2_GSE_HDR_LENGTH_MASK) - (new_off - DVB_S2_GSE_MINSIZE);
        }

        data_tvb = NULL;
        if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_START_POS) || BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_STOP_POS)) {
            fragment_head *dvbs2_frag_head = NULL;
            int offset = cur_off + new_off;
            if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_START_POS)) {
                offset -= 2; /* re-include GSE type in reassembled data */
                data_len += 2;
            }
            dvbs2_frag_head = fragment_add_seq_next(&dvbs2_reassembly_table, tvb, offset,
                pinfo, fragid, NULL, data_len, BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_STOP_POS));

            if (BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_STOP_POS))
                dvbs2_frag_head = fragment_end_seq_next(&dvbs2_reassembly_table, pinfo, fragid, NULL);

            data_tvb = process_reassembled_data(tvb, cur_off + new_off, pinfo, "Reassembled DVB-S2",
                dvbs2_frag_head, &dvbs2_frag_items, &update_col_info, tree);
        }

        if (data_tvb != NULL) {
            /* We have a reassembled packet. Extract the gse_proto from it. */
            gse_proto = tvb_get_ntohs(data_tvb, 0);
            /* And then remove it from the reassembled data */
            data_tvb = tvb_new_subset_remaining(data_tvb, 2);
        } else {
            data_tvb = tvb_new_subset_length(tvb, cur_off + new_off, data_len);
        }

        switch (gse_proto) {
            case ETHERTYPE_IP:
                if (dvb_s2_full_dissection)
                {
                    new_off += call_dissector(ip_handle, next_tvb, pinfo, tree);
                    dissected = TRUE;
                }
                break;

            case ETHERTYPE_IPv6:
                if (dvb_s2_full_dissection)
                {
                    new_off += call_dissector(ipv6_handle, next_tvb, pinfo, tree);
                    dissected = TRUE;
                }
                break;

            case ETHERTYPE_VLAN:
                if (dvb_s2_full_dissection)
                {
                    new_off += call_dissector(eth_withoutfcs_handle, next_tvb, pinfo, tree);
                    dissected = TRUE;
                }
                break;

            case DVB_RCS2_SIGNAL_TABLE:
                call_dissector(dvb_s2_table_handle, data_tvb, pinfo, tree);
                new_off += data_len;
                dissected = TRUE;
                break;

            case DVB_RCS2_NCR:
                ttf = proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_ncr, tvb, cur_off + new_off, data_len, ENC_NA);
                dvb_s2_gse_ncr_tree = proto_item_add_subtree(ttf, ett_dvb_s2_gse_ncr);
                proto_tree_add_item(dvb_s2_gse_ncr_tree, hf_dvb_s2_gse_data, tvb, cur_off + new_off, data_len, ENC_NA);
                new_off += data_len;
                dissected = TRUE;
                break;

            default:
                /* Not handled! TODO: expert info? */
                break;
        }

        if (!dissected) {
            proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_data, tvb, cur_off + new_off, data_len, ENC_NA);
            new_off += data_len;
        }

        /* add crc32 if last fragment */
        if (BIT_IS_CLEAR(gse_hdr, DVB_S2_GSE_HDR_START_POS) && BIT_IS_SET(gse_hdr, DVB_S2_GSE_HDR_STOP_POS)) {
            proto_tree_add_item(dvb_s2_gse_tree, hf_dvb_s2_gse_crc32, tvb, cur_off + new_off, DVB_S2_GSE_CRC32_LEN, ENC_BIG_ENDIAN);
            new_off += DVB_S2_GSE_CRC32_LEN;
        }
    }

    return new_off;
}