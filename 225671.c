dissect_tcpopt_scps(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    struct tcp_analysis *tcpd;
    proto_tree *field_tree = NULL;
    tcp_flow_t *flow;
    int         direction;
    proto_item *tf = NULL, *item;
    proto_tree *flags_tree = NULL;
    guint8      capvector;
    guint8      connid;
    int         offset = 0, optlen = tvb_reported_length(tvb);

    tcpd = get_tcp_conversation_data(NULL,pinfo);

    /* check direction and get ua lists */
    direction=cmp_address(&pinfo->src, &pinfo->dst);

    /* if the addresses are equal, match the ports instead */
    if(direction==0) {
        direction= (pinfo->srcport > pinfo->destport) ? 1 : -1;
    }

    if(direction>=0)
        flow =&(tcpd->flow1);
    else
        flow =&(tcpd->flow2);

    item = proto_tree_add_item(tree, proto_tcp_option_scps,
                               tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(item, ett_tcp_option_scps);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                        offset + 1, 1, ENC_BIG_ENDIAN);

    /* If the option length == 4, this is a real SCPS capability option
     * See "CCSDS 714.0-B-2 (CCSDS Recommended Standard for SCPS Transport Protocol
     * (SCPS-TP)" Section 3.2.3 for definition.
     */
    if (optlen == 4) {
        tf = proto_tree_add_item(field_tree, hf_tcp_option_scps_vector, tvb,
                                 offset + 2, 1, ENC_BIG_ENDIAN);
        flags_tree = proto_item_add_subtree(tf, ett_tcp_scpsoption_flags);
        proto_tree_add_item(flags_tree, hf_tcp_scpsoption_flags_bets, tvb,
                            offset + 2, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(flags_tree, hf_tcp_scpsoption_flags_snack1, tvb,
                            offset + 2, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(flags_tree, hf_tcp_scpsoption_flags_snack2, tvb,
                            offset + 2, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(flags_tree, hf_tcp_scpsoption_flags_compress, tvb,
                            offset + 2, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(flags_tree, hf_tcp_scpsoption_flags_nlts, tvb,
                            offset + 2, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(flags_tree, hf_tcp_scpsoption_flags_reserved, tvb,
                            offset + 2, 1, ENC_BIG_ENDIAN);
        capvector = tvb_get_guint8(tvb, offset + 2);

        if (capvector) {
            struct capvec
            {
                guint8 mask;
                const gchar *str;
            } capvecs[] = {
                {0x80, "BETS"},
                {0x40, "SNACK1"},
                {0x20, "SNACK2"},
                {0x10, "COMP"},
                {0x08, "NLTS"},
                {0x07, "RESERVED"}
            };
            gboolean anyflag = FALSE;
            guint i;

            col_append_str(pinfo->cinfo, COL_INFO, " SCPS[");
            for (i = 0; i < sizeof(capvecs)/sizeof(struct capvec); i++) {
                if (capvector & capvecs[i].mask) {
                    proto_item_append_text(tf, "%s%s", anyflag ? ", " : " (",
                                           capvecs[i].str);
                    col_append_lstr(pinfo->cinfo, COL_INFO,
                                    anyflag ? ", " : "",
                                    capvecs[i].str,
                                    COL_ADD_LSTR_TERMINATOR);
                    anyflag = TRUE;
                }
            }
            col_append_str(pinfo->cinfo, COL_INFO, "]");
            proto_item_append_text(tf, ")");
        }

        proto_tree_add_item(field_tree, hf_tcp_scpsoption_connection_id, tvb,
                            offset + 3, 1, ENC_BIG_ENDIAN);
        connid = tvb_get_guint8(tvb, offset + 3);
        flow->scps_capable = 1;

        if (connid)
            tcp_info_append_uint(pinfo, "Connection ID", connid);
    } else {
        /* The option length != 4, so this is an infamous "extended capabilities
         * option. See "CCSDS 714.0-B-2 (CCSDS Recommended Standard for SCPS
         * Transport Protocol (SCPS-TP)" Section 3.2.5 for definition.
         *
         *  As the format of this option is only partially defined (it is
         * a community (or more likely vendor) defined format beyond that, so
         * at least for now, we only parse the standardized portion of the option.
         */
        guint8 local_offset = 2;
        guint8 binding_space;
        guint8 extended_cap_length;

        if (flow->scps_capable != 1) {
            /* There was no SCPS capabilities option preceding this */
            proto_item_set_text(item,
                                "Illegal SCPS Extended Capabilities (%u bytes)",
                                optlen);
        } else {
            proto_item_set_text(item,
                                "SCPS Extended Capabilities (%u bytes)",
                                optlen);

            /* There may be multiple binding spaces included in a single option,
             * so we will semi-parse each of the stacked binding spaces - skipping
             * over the octets following the binding space identifier and length.
             */
            while (optlen > local_offset) {

                /* 1st octet is Extended Capability Binding Space */
                binding_space = tvb_get_guint8(tvb, (offset + local_offset));

                /* 2nd octet (upper 4-bits) has binding space length in 16-bit words.
                 * As defined by the specification, this length is exclusive of the
                 * octets containing the extended capability type and length
                 */
                extended_cap_length =
                    (tvb_get_guint8(tvb, (offset + local_offset + 1)) >> 4);

                /* Convert the extended capabilities length into bytes for display */
                extended_cap_length = (extended_cap_length << 1);

                proto_tree_add_item(field_tree, hf_tcp_option_scps_binding, tvb, offset + local_offset, 1, ENC_BIG_ENDIAN);
                proto_tree_add_uint(field_tree, hf_tcp_option_scps_binding_len, tvb, offset + local_offset + 1, 1, extended_cap_length);

                /* Step past the binding space and length octets */
                local_offset += 2;

                proto_tree_add_item(field_tree, hf_tcp_option_scps_binding_data, tvb, offset + local_offset, extended_cap_length, ENC_NA);

                tcp_info_append_uint(pinfo, "EXCAP", binding_space);

                /* Step past the Extended capability data
                 * Treat the extended capability data area as opaque;
                 * If one desires to parse the extended capability data
                 * (say, in a vendor aware build of wireshark), it would
                 * be triggered here.
                 */
                local_offset += extended_cap_length;
            }
        }
    }

    return tvb_captured_length(tvb);
}