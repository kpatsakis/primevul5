dissect_tcpopt_mptcp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
    proto_item *item,*main_item;
    proto_tree *mptcp_tree;

    guint8 subtype;
    guint8 ipver;
    int offset = 0;
    int optlen = tvb_reported_length(tvb);
    int start_offset = offset;
    struct tcp_analysis *tcpd = NULL;
    struct mptcp_analysis* mptcpd = NULL;
    struct tcpheader *tcph = (struct tcpheader *)data;

    /* There may be several MPTCP options per packet, don't duplicate the structure */
    struct mptcpheader* mph = tcph->th_mptcp;

    if(!mph) {
        mph = wmem_new0(wmem_packet_scope(), struct mptcpheader);
        tcph->th_mptcp = mph;
    }

    tcpd=get_tcp_conversation_data(NULL,pinfo);
    mptcpd=tcpd->mptcp_analysis;

    /* seeing an MPTCP packet on the subflow automatically qualifies it as an mptcp subflow */
    if(!tcpd->fwd->mptcp_subflow) {
         mptcp_init_subflow(tcpd->fwd);
    }
    if(!tcpd->rev->mptcp_subflow) {
         mptcp_init_subflow(tcpd->rev);
    }

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "MPTCP");
    main_item = proto_tree_add_item(tree, proto_mptcp, tvb, offset, -1, ENC_NA);
    mptcp_tree = proto_item_add_subtree(main_item, ett_tcp_option_mptcp);

    proto_tree_add_item(mptcp_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    proto_tree_add_item(mptcp_tree, hf_tcp_option_len, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_subtype, tvb,
                        offset, 1, ENC_BIG_ENDIAN);

    subtype = tvb_get_guint8(tvb, offset) >> 4;
    proto_item_append_text(main_item, ": %s", val_to_str(subtype, mptcp_subtype_vs, "Unknown (%d)"));

    /** preemptively allocate mptcpd when subtype won't allow to find a meta */
    if(!mptcpd && (subtype > TCPOPT_MPTCP_MP_JOIN)) {
        mptcpd = mptcp_alloc_analysis(tcpd);
    }

    switch (subtype) {
        case TCPOPT_MPTCP_MP_CAPABLE:
            mph->mh_mpc = TRUE;

            proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_version, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
            offset += 1;

            item = proto_tree_add_bitmask(mptcp_tree, tvb, offset, hf_tcp_option_mptcp_flags,
                         ett_tcp_option_mptcp, tcp_option_mptcp_capable_flags,
                         ENC_BIG_ENDIAN);
            mph->mh_capable_flags = tvb_get_guint8(tvb, offset);
            if ((mph->mh_capable_flags & MPTCP_CAPABLE_CRYPTO_MASK) == 0) {
                expert_add_info(pinfo, item, &ei_mptcp_analysis_missing_algorithm);
            }
            if ((mph->mh_capable_flags & MPTCP_CAPABLE_CRYPTO_MASK) != MPTCP_HMAC_SHA1) {
                expert_add_info(pinfo, item, &ei_mptcp_analysis_unsupported_algorithm);
            }
            offset += 1;

            /* optlen == 12 => SYN or SYN/ACK; optlen == 20 => ACK */
            if (optlen == 12 || optlen == 20) {

                mph->mh_key = tvb_get_ntoh64(tvb,offset);
                proto_tree_add_uint64(mptcp_tree, hf_tcp_option_mptcp_sender_key, tvb, offset, 8, mph->mh_key);
                offset += 8;

                mptcpd = get_or_create_mptcpd_from_key(tcpd, tcpd->fwd, mph->mh_key, mph->mh_capable_flags & MPTCP_CAPABLE_CRYPTO_MASK);
                mptcpd->master = tcpd;

                item = proto_tree_add_uint(mptcp_tree,
                      hf_mptcp_expected_token, tvb, offset, 0, tcpd->fwd->mptcp_subflow->meta->token);
                PROTO_ITEM_SET_GENERATED(item);

                item = proto_tree_add_uint64(mptcp_tree,
                      hf_mptcp_expected_idsn, tvb, offset, 0, tcpd->fwd->mptcp_subflow->meta->base_dsn);
                PROTO_ITEM_SET_GENERATED(item);

                /* last ACK of 3WHS, repeats both keys */
                if (optlen == 20) {
                    guint64 recv_key = tvb_get_ntoh64(tvb,offset);
                    proto_tree_add_uint64(mptcp_tree, hf_tcp_option_mptcp_recv_key, tvb, offset, 8, recv_key);

                    if(tcpd->rev->mptcp_subflow->meta
                        && (tcpd->rev->mptcp_subflow->meta->static_flags & MPTCP_META_HAS_KEY)) {

                        /* compare the echoed key with the server key */
                        if(tcpd->rev->mptcp_subflow->meta->key != recv_key) {
                            expert_add_info(pinfo, item, &ei_mptcp_analysis_echoed_key_mismatch);
                        }
                    }
                    else {
                        mptcpd = get_or_create_mptcpd_from_key(tcpd, tcpd->rev, recv_key, mph->mh_capable_flags & MPTCP_CAPABLE_CRYPTO_MASK);
                    }
                }
            }
            break;

        case TCPOPT_MPTCP_MP_JOIN:
            mph->mh_join = TRUE;
            if(optlen != 12 && !mptcpd) {
                mptcpd = mptcp_alloc_analysis(tcpd);
            }
            switch (optlen) {
                /* Syn */
                case 12:
                    {
                    proto_tree_add_bitmask(mptcp_tree, tvb, offset, hf_tcp_option_mptcp_flags,
                         ett_tcp_option_mptcp, tcp_option_mptcp_join_flags,
                         ENC_BIG_ENDIAN);
                    offset += 1;
                    tcpd->fwd->mptcp_subflow->address_id = tvb_get_guint8(tvb, offset);
                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_address_id, tvb, offset,
                            1, ENC_BIG_ENDIAN);
                    offset += 1;

                    proto_tree_add_item_ret_uint(mptcp_tree, hf_tcp_option_mptcp_recv_token, tvb, offset,
                            4, ENC_BIG_ENDIAN, &mph->mh_token);
                    offset += 4;

                    mptcpd = mptcp_get_meta_from_token(tcpd, tcpd->rev, mph->mh_token);

                    proto_tree_add_item_ret_uint(mptcp_tree, hf_tcp_option_mptcp_sender_rand, tvb, offset,
                            4, ENC_BIG_ENDIAN, &tcpd->fwd->mptcp_subflow->nonce);

                    }
                    break;


                case 16:    /* Syn/Ack */
                    proto_tree_add_bitmask(mptcp_tree, tvb, offset, hf_tcp_option_mptcp_flags,
                         ett_tcp_option_mptcp, tcp_option_mptcp_join_flags,
                         ENC_BIG_ENDIAN);
                    offset += 1;

                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_address_id, tvb, offset,
                            1, ENC_BIG_ENDIAN);
                    offset += 1;

                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_sender_trunc_hmac, tvb, offset,
                            8, ENC_BIG_ENDIAN);
                    offset += 8;

                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_sender_rand, tvb, offset,
                            4, ENC_BIG_ENDIAN);
                    break;

                case 24:    /* Ack */
                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_reserved, tvb, offset,
                            2, ENC_BIG_ENDIAN);
                    offset += 2;

                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_sender_hmac, tvb, offset,
                                20, ENC_NA);
                    break;

                default:
                    break;
            }
            break;

        /* display only *raw* values since it is harder to guess a correct value than for TCP.
        One needs to enable mptcp_analysis to get more interesting data
         */
        case TCPOPT_MPTCP_DSS:
            mph->mh_dss = TRUE;

            offset += 1;
            mph->mh_dss_flags = tvb_get_guint8(tvb, offset) & 0x1F;

            proto_tree_add_bitmask(mptcp_tree, tvb, offset, hf_tcp_option_mptcp_flags,
                         ett_tcp_option_mptcp, tcp_option_mptcp_dss_flags,
                         ENC_BIG_ENDIAN);
            offset += 1;

            /* displays "raw" DataAck , ie does not convert it to its 64 bits form
            to do so you need to enable
            */
            if (mph->mh_dss_flags & MPTCP_DSS_FLAG_DATA_ACK_PRESENT) {

                guint64 dack64;

                /* 64bits ack */
                if (mph->mh_dss_flags & MPTCP_DSS_FLAG_DATA_ACK_8BYTES) {

                    mph->mh_dss_rawack = tvb_get_ntoh64(tvb,offset);
                    proto_tree_add_uint64_format_value(mptcp_tree, hf_tcp_option_mptcp_data_ack_raw, tvb, offset, 8, mph->mh_dss_rawack, "%" G_GINT64_MODIFIER "u (64bits)", mph->mh_dss_rawack);
                    offset += 8;
                }
                /* 32bits ack */
                else {
                    mph->mh_dss_rawack = tvb_get_ntohl(tvb,offset);
                    proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_data_ack_raw, tvb, offset, 4, ENC_BIG_ENDIAN);
                    offset += 4;
                }

                if(mptcp_convert_dsn(mph->mh_dss_rawack, tcpd->rev->mptcp_subflow->meta,
                    (mph->mh_dss_flags & MPTCP_DSS_FLAG_DATA_ACK_8BYTES) ? DSN_CONV_NONE : DSN_CONV_32_TO_64, mptcp_relative_seq, &dack64)) {
                    item = proto_tree_add_uint64(mptcp_tree, hf_mptcp_ack, tvb, 0, 0, dack64);
                    if (mptcp_relative_seq) {
                        proto_item_append_text(item, " (Relative)");
                    }

                    PROTO_ITEM_SET_GENERATED(item);
                }
                else {
                    /* ignore and continue */
                }

            }

            /* Mapping present */
            if (mph->mh_dss_flags & MPTCP_DSS_FLAG_MAPPING_PRESENT) {

                guint64 dsn;

                if (mph->mh_dss_flags & MPTCP_DSS_FLAG_DSN_8BYTES) {

                    dsn = tvb_get_ntoh64(tvb,offset);
                    proto_tree_add_uint64_format_value(mptcp_tree, hf_tcp_option_mptcp_data_seq_no_raw, tvb, offset, 8, dsn,  "%" G_GINT64_MODIFIER "u  (64bits version)", dsn);

                    /* if we have the opportunity to complete the 32 Most Significant Bits of the
                     *
                     */
                    if(!(tcpd->fwd->mptcp_subflow->meta->static_flags & MPTCP_META_HAS_BASE_DSN_MSB)) {
                        tcpd->fwd->mptcp_subflow->meta->static_flags |= MPTCP_META_HAS_BASE_DSN_MSB;
                        tcpd->fwd->mptcp_subflow->meta->base_dsn |= (dsn & (guint32) 0);
                    }
                    offset += 8;
                } else {
                    dsn = tvb_get_ntohl(tvb,offset);
                    proto_tree_add_uint64_format_value(mptcp_tree, hf_tcp_option_mptcp_data_seq_no_raw, tvb, offset, 4, dsn,  "%" G_GINT64_MODIFIER "u  (32bits version)", dsn);
                    offset += 4;
                }
                mph->mh_dss_rawdsn = dsn;

                proto_tree_add_item_ret_uint(mptcp_tree, hf_tcp_option_mptcp_subflow_seq_no, tvb, offset, 4, ENC_BIG_ENDIAN, &mph->mh_dss_ssn);
                offset += 4;

                proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_data_lvl_len, tvb, offset, 2, ENC_BIG_ENDIAN);
                mph->mh_dss_length = tvb_get_ntohs(tvb,offset);
                offset += 2;

                if(mph->mh_dss_length == 0) {
                    expert_add_info(pinfo, mptcp_tree, &ei_mptcp_infinite_mapping);
                }

                /* print head & tail dsn */
                if(mptcp_convert_dsn(mph->mh_dss_rawdsn, tcpd->fwd->mptcp_subflow->meta,
                    (mph->mh_dss_flags & MPTCP_DSS_FLAG_DATA_ACK_8BYTES) ? DSN_CONV_NONE : DSN_CONV_32_TO_64, mptcp_relative_seq, &dsn)) {
                    item = proto_tree_add_uint64(mptcp_tree, hf_mptcp_dss_dsn, tvb, 0, 0, dsn);
                    if (mptcp_relative_seq) {
                            proto_item_append_text(item, " (Relative)");
                    }

                    PROTO_ITEM_SET_GENERATED(item);
                }
                else {
                    /* ignore and continue */
                }

                /* if mapping analysis enabled and not a */
                if(mptcp_analyze_mappings && mph->mh_dss_length)
                {

                    if (!PINFO_FD_VISITED(pinfo))
                    {
                        /* register SSN range described by the mapping into a subflow interval_tree */
                        mptcp_dss_mapping_t *mapping = NULL;
                        mapping = wmem_new0(wmem_file_scope(), mptcp_dss_mapping_t);

                        mapping->rawdsn  = mph->mh_dss_rawdsn;
                        mapping->extended_dsn = (mph->mh_dss_flags & MPTCP_DSS_FLAG_DATA_ACK_8BYTES);
                        mapping->frame = pinfo->fd->num;
                        mapping->ssn_low = mph->mh_dss_ssn;
                        mapping->ssn_high = mph->mh_dss_ssn + mph->mh_dss_length-1;

                        wmem_itree_insert(tcpd->fwd->mptcp_subflow->ssn2dsn_mappings,
                            mph->mh_dss_ssn,
                            mapping->ssn_high,
                            mapping
                            );
                    }
                }

                if ((int)optlen >= offset-start_offset+4)
                {
                    proto_tree_add_checksum(mptcp_tree, tvb, offset, hf_tcp_option_mptcp_checksum, -1, NULL, pinfo, 0, ENC_BIG_ENDIAN, PROTO_CHECKSUM_NO_FLAGS);
                }
            }
            break;

        case TCPOPT_MPTCP_ADD_ADDR:
            proto_tree_add_item(mptcp_tree,
                            hf_tcp_option_mptcp_ipver, tvb, offset, 1, ENC_BIG_ENDIAN);
            ipver = tvb_get_guint8(tvb, offset) & 0x0F;
            offset += 1;

            proto_tree_add_item(mptcp_tree,
                    hf_tcp_option_mptcp_address_id, tvb, offset, 1, ENC_BIG_ENDIAN);
            offset += 1;

            switch (ipver) {
                case 4:
                    proto_tree_add_item(mptcp_tree,
                            hf_tcp_option_mptcp_ipv4, tvb, offset, 4, ENC_BIG_ENDIAN);
                    offset += 4;
                    break;

                case 6:
                    proto_tree_add_item(mptcp_tree,
                            hf_tcp_option_mptcp_ipv6, tvb, offset, 16, ENC_NA);
                    offset += 16;
                    break;

                default:
                    break;
            }

            if (optlen % 4 == 2) {
                proto_tree_add_item(mptcp_tree,
                            hf_tcp_option_mptcp_port, tvb, offset, 2, ENC_BIG_ENDIAN);
                offset += 2;
            }

            if (optlen == 16 || optlen == 18 || optlen == 28 || optlen == 30) {
                proto_tree_add_item(mptcp_tree,
                            hf_tcp_option_mptcp_addaddr_trunc_hmac, tvb, offset, 8, ENC_BIG_ENDIAN);
            }
            break;

        case TCPOPT_MPTCP_REMOVE_ADDR:
            item = proto_tree_add_uint(mptcp_tree, hf_mptcp_number_of_removed_addresses, tvb, start_offset+2,
                1, optlen - 3);
            PROTO_ITEM_SET_GENERATED(item);
            offset += 1;
            while(offset < start_offset + (int)optlen) {
                proto_tree_add_item(mptcp_tree, hf_tcp_option_mptcp_address_id, tvb, offset,
                                1, ENC_BIG_ENDIAN);
                offset += 1;
            }
            break;

        case TCPOPT_MPTCP_MP_PRIO:
            proto_tree_add_bitmask(mptcp_tree, tvb, offset, hf_tcp_option_mptcp_flags,
                         ett_tcp_option_mptcp, tcp_option_mptcp_join_flags,
                         ENC_BIG_ENDIAN);
            offset += 1;

            if (optlen == 4) {
                proto_tree_add_item(mptcp_tree,
                        hf_tcp_option_mptcp_address_id, tvb, offset, 1, ENC_BIG_ENDIAN);
            }
            break;

        case TCPOPT_MPTCP_MP_FAIL:
            mph->mh_fail = TRUE;
            proto_tree_add_item(mptcp_tree,
                    hf_tcp_option_mptcp_reserved, tvb, offset,2, ENC_BIG_ENDIAN);
            offset += 2;

            proto_tree_add_item(mptcp_tree,
                    hf_tcp_option_mptcp_data_seq_no_raw, tvb, offset, 8, ENC_BIG_ENDIAN);
            break;

        case TCPOPT_MPTCP_MP_FASTCLOSE:
            mph->mh_fastclose = TRUE;
            proto_tree_add_item(mptcp_tree,
                    hf_tcp_option_mptcp_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
            offset += 2;

            proto_tree_add_item(mptcp_tree,
                    hf_tcp_option_mptcp_recv_key, tvb, offset, 8, ENC_BIG_ENDIAN);
            mph->mh_key = tvb_get_ntoh64(tvb,offset);
            break;

        default:
            break;
    }

    if ((mptcpd != NULL) && (tcpd->mptcp_analysis != NULL)) {

        /* if mptcpd just got allocated, remember the initial addresses
         * which will serve as identifiers for the conversation filter
         */
        if(tcpd->fwd->mptcp_subflow->meta->ip_src.len == 0) {

            copy_address_wmem(wmem_file_scope(), &tcpd->fwd->mptcp_subflow->meta->ip_src, &tcph->ip_src);
            copy_address_wmem(wmem_file_scope(), &tcpd->fwd->mptcp_subflow->meta->ip_dst, &tcph->ip_dst);

            copy_address_shallow(&tcpd->rev->mptcp_subflow->meta->ip_src, &tcpd->fwd->mptcp_subflow->meta->ip_dst);
            copy_address_shallow(&tcpd->rev->mptcp_subflow->meta->ip_dst, &tcpd->fwd->mptcp_subflow->meta->ip_src);

            tcpd->fwd->mptcp_subflow->meta->sport = tcph->th_sport;
            tcpd->fwd->mptcp_subflow->meta->dport = tcph->th_dport;
        }

        mph->mh_stream = tcpd->mptcp_analysis->stream;
    }

    return tvb_captured_length(tvb);
}