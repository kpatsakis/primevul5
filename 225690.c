dissect_tcpopt_rvbd_probe(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
    guint8 ver, type;
    proto_tree *field_tree;
    proto_item *pitem;
    proto_item *length_item;
    int offset = 0,
        optlen = tvb_reported_length(tvb);
    struct tcpheader *tcph = (struct tcpheader*)data;

    pitem = proto_tree_add_item(tree, proto_tcp_option_rvbd_probe, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(pitem, ett_tcp_opt_rvbd_probe);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                                      offset + 1, 1, ENC_BIG_ENDIAN);

    if (optlen < TCPOLEN_RVBD_PROBE_MIN) {
        /* Bogus - option length is less than what it's supposed to be for
           this option. */
        expert_add_info_format(pinfo, length_item, &ei_tcp_opt_len_invalid,
                            "option length should be >= %u)",
                            TCPOLEN_RVBD_PROBE_MIN);
        return tvb_captured_length(tvb);
    }

    rvbd_probe_decode_version_type(
        tvb_get_guint8(tvb, offset + PROBE_VERSION_TYPE_OFFSET),
        &ver, &type);

    proto_item_append_text(pitem, ": %s", val_to_str_const(type, rvbd_probe_type_vs, "Probe Unknown"));

    if (type >= PROBE_TYPE_MAX)
        return tvb_captured_length(tvb);

    if (ver == PROBE_VERSION_1) {
        guint16 port;

        proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_type1, tvb,
                            offset + PROBE_VERSION_TYPE_OFFSET, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_version1, tvb,
                            offset + PROBE_VERSION_TYPE_OFFSET, 1, ENC_BIG_ENDIAN);

        if (type == PROBE_INTERNAL)
            return offset + PROBE_VERSION_TYPE_OFFSET;

        proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_reserved, tvb, offset + PROBE_V1_RESERVED_OFFSET, 1, ENC_BIG_ENDIAN);

        proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_prober, tvb,
                            offset + PROBE_V1_PROBER_OFFSET, 4, ENC_BIG_ENDIAN);

        switch (type) {

        case PROBE_QUERY:
        case PROBE_QUERY_SH:
        case PROBE_TRACE:
            {
            rvbd_option_data* option_data;
            proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_appli_ver, tvb,
                                offset + PROBE_V1_APPLI_VERSION_OFFSET, 2,
                                ENC_BIG_ENDIAN);

            proto_item_append_text(pitem, ", CSH IP: %s", tvb_ip_to_str(tvb, offset + PROBE_V1_PROBER_OFFSET));

            option_data = (rvbd_option_data*)p_get_proto_data(pinfo->pool, pinfo, proto_tcp_option_rvbd_probe, pinfo->curr_layer_num);
            if (option_data == NULL)
            {
                option_data = wmem_new0(pinfo->pool, rvbd_option_data);
                p_add_proto_data(pinfo->pool, pinfo, proto_tcp_option_rvbd_probe, pinfo->curr_layer_num, option_data);
            }

            option_data->valid = TRUE;
            option_data->type = type;

            }
            break;

        case PROBE_RESPONSE:
            proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_proxy, tvb,
                                offset + PROBE_V1_PROXY_ADDR_OFFSET, 4, ENC_BIG_ENDIAN);

            port = tvb_get_ntohs(tvb, offset + PROBE_V1_PROXY_PORT_OFFSET);
            proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_proxy_port, tvb,
                                offset + PROBE_V1_PROXY_PORT_OFFSET, 2, ENC_BIG_ENDIAN);

            rvbd_probe_resp_add_info(pitem, pinfo, tvb, offset + PROBE_V1_PROXY_ADDR_OFFSET, port);
            break;

        case PROBE_RESPONSE_SH:
            proto_tree_add_item(field_tree,
                                hf_tcp_option_rvbd_probe_client, tvb,
                                offset + PROBE_V1_SH_CLIENT_ADDR_OFFSET, 4,
                                ENC_BIG_ENDIAN);

            proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_proxy, tvb,
                                offset + PROBE_V1_SH_PROXY_ADDR_OFFSET, 4, ENC_BIG_ENDIAN);

            port = tvb_get_ntohs(tvb, offset + PROBE_V1_SH_PROXY_PORT_OFFSET);
            proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_proxy_port, tvb,
                                offset + PROBE_V1_SH_PROXY_PORT_OFFSET, 2, ENC_BIG_ENDIAN);

            rvbd_probe_resp_add_info(pitem, pinfo, tvb, offset + PROBE_V1_SH_PROXY_ADDR_OFFSET, port);
            break;
        }
    }
    else if (ver == PROBE_VERSION_2) {
        proto_item *ver_pi;
        proto_item *flag_pi;
        proto_tree *flag_tree;
        guint8 flags;

        proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_type2, tvb,
                            offset + PROBE_VERSION_TYPE_OFFSET, 1, ENC_BIG_ENDIAN);

        proto_tree_add_uint_format_value(
            field_tree, hf_tcp_option_rvbd_probe_version2, tvb,
            offset + PROBE_VERSION_TYPE_OFFSET, 1, ver, "%u", ver);
        /* Use version1 for filtering purposes because version2 packet
           value is 0, but filtering is usually done for value 2 */
        ver_pi = proto_tree_add_uint(field_tree, hf_tcp_option_rvbd_probe_version1, tvb,
                                     offset + PROBE_VERSION_TYPE_OFFSET, 1, ver);
        PROTO_ITEM_SET_HIDDEN(ver_pi);

        switch (type) {

        case PROBE_QUERY_INFO:
        case PROBE_QUERY_INFO_SH:
        case PROBE_QUERY_INFO_SID:
            flags = tvb_get_guint8(tvb, offset + PROBE_V2_INFO_OFFSET);
            flag_pi = proto_tree_add_uint(field_tree, hf_tcp_option_rvbd_probe_flags,
                                          tvb, offset + PROBE_V2_INFO_OFFSET,
                                          1, flags);

            flag_tree = proto_item_add_subtree(flag_pi, ett_tcp_opt_rvbd_probe_flags);
            proto_tree_add_item(flag_tree,
                                hf_tcp_option_rvbd_probe_flag_not_cfe,
                                tvb, offset + PROBE_V2_INFO_OFFSET, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(flag_tree,
                                hf_tcp_option_rvbd_probe_flag_last_notify,
                                tvb, offset + PROBE_V2_INFO_OFFSET, 1, ENC_BIG_ENDIAN);

            switch (type)
            {
            case PROBE_QUERY_INFO:
                {
                    rvbd_option_data* option_data = (rvbd_option_data*)p_get_proto_data(pinfo->pool, pinfo, proto_tcp_option_rvbd_probe, pinfo->curr_layer_num);
                    if (option_data == NULL)
                    {
                        option_data = wmem_new0(pinfo->pool, rvbd_option_data);
                        p_add_proto_data(pinfo->pool, pinfo, proto_tcp_option_rvbd_probe, pinfo->curr_layer_num, option_data);
                    }

                    option_data->probe_flags = flags;
                }
                break;
            case PROBE_QUERY_INFO_SH:
                proto_tree_add_item(flag_tree,
                                    hf_tcp_option_rvbd_probe_client, tvb,
                                    offset + PROBE_V2_INFO_CLIENT_ADDR_OFFSET,
                                    4, ENC_BIG_ENDIAN);
                break;
            case PROBE_QUERY_INFO_SID:
                proto_tree_add_item(flag_tree,
                                    hf_tcp_option_rvbd_probe_storeid, tvb,
                                    offset + PROBE_V2_INFO_STOREID_OFFSET,
                                    4, ENC_BIG_ENDIAN);
                break;
            }

            if (type != PROBE_QUERY_INFO_SID &&
                tcph != NULL &&
                (tcph->th_flags & (TH_SYN|TH_ACK)) == (TH_SYN|TH_ACK) &&
                (flags & RVBD_FLAGS_PROBE_LAST)) {
                col_prepend_fstr(pinfo->cinfo, COL_INFO, "SA++, ");
            }

            break;

        case PROBE_RESPONSE_INFO:
            flag_pi = proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_flags,
                                          tvb, offset + PROBE_V2_INFO_OFFSET,
                                          1, ENC_BIG_ENDIAN);

            flag_tree = proto_item_add_subtree(flag_pi, ett_tcp_opt_rvbd_probe_flags);
            proto_tree_add_item(flag_tree,
                                hf_tcp_option_rvbd_probe_flag_probe_cache,
                                tvb, offset + PROBE_V2_INFO_OFFSET, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(flag_tree,
                                hf_tcp_option_rvbd_probe_flag_sslcert,
                                tvb, offset + PROBE_V2_INFO_OFFSET, 1, ENC_BIG_ENDIAN);
            proto_tree_add_item(flag_tree,
                                hf_tcp_option_rvbd_probe_flag_server_connected,
                                tvb, offset + PROBE_V2_INFO_OFFSET, 1, ENC_BIG_ENDIAN);
            break;

        case PROBE_RST:
            proto_tree_add_item(field_tree, hf_tcp_option_rvbd_probe_flags,
                                  tvb, offset + PROBE_V2_INFO_OFFSET,
                                  1, ENC_BIG_ENDIAN);
            break;
        }
    }

    return tvb_captured_length(tvb);
}