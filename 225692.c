tcp_dissect_options(tvbuff_t *tvb, int offset, guint length, int eol,
                       packet_info *pinfo, proto_tree *opt_tree,
                       proto_item *opt_item, void * data)
{
    guchar            opt;
    guint             optlen, nop_count = 0;
    proto_tree       *field_tree;
    const char       *name;
    dissector_handle_t option_dissector;
    tvbuff_t         *next_tvb;

    while (length > 0) {
        opt = tvb_get_guint8(tvb, offset);
        --length;      /* account for type byte */
        if ((opt == TCPOPT_EOL) || (opt == TCPOPT_NOP)) {
            int local_proto;
            proto_item* field_item;

            /* We assume that the only options with no length are EOL and
               NOP options, so that we can treat unknown options as having
               a minimum length of 2, and at least be able to move on to
               the next option by using the length in the option. */
            if (opt == TCPOPT_EOL) {
                local_proto = proto_tcp_option_eol;
            } else if (opt == TCPOPT_NOP) {
                local_proto = proto_tcp_option_nop;

                if (opt_item && (nop_count == 0 || offset % 4)) {
                    /* Count number of NOP in a row within a uint32 */
                    nop_count++;

                    if (nop_count == 4) {
                        expert_add_info(pinfo, opt_item, &ei_tcp_nop);
                    }
                } else {
                    nop_count = 0;
                }
            } else {
                g_assert_not_reached();
            }

            field_item = proto_tree_add_item(opt_tree, local_proto, tvb, offset, 1, ENC_NA);
            field_tree = proto_item_add_subtree(field_item, ett_tcp_option_other);
            proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
            proto_item_append_text(proto_tree_get_parent(opt_tree), ", %s", proto_get_protocol_short_name(find_protocol_by_id(local_proto)));
            offset += 1;
        } else {
            option_dissector = dissector_get_uint_handle(tcp_option_table, opt);
            if (option_dissector == NULL) {
                name = wmem_strdup_printf(wmem_packet_scope(), "Unknown (0x%02x)", opt);
                option_dissector = tcp_opt_unknown_handle;
            } else {
                name = dissector_handle_get_short_name(option_dissector);
            }

            /* Option has a length. Is it in the packet? */
            if (length == 0) {
                /* Bogus - packet must at least include option code byte and
                    length byte! */
                proto_tree_add_expert_format(opt_tree, pinfo, &ei_tcp_opt_len_invalid, tvb, offset, 1,
                                                "%s (length byte past end of options)", name);
                return;
            }

            optlen = tvb_get_guint8(tvb, offset + 1);  /* total including type, len */
            --length;    /* account for length byte */

            if (optlen < 2) {
                /* Bogus - option length is too short to include option code and
                    option length. */
                proto_tree_add_expert_format(opt_tree, pinfo, &ei_tcp_opt_len_invalid, tvb, offset, 2,
                                    "%s (with too-short option length = %u byte%s)",
                                    name, optlen, plurality(optlen, "", "s"));
                return;
            } else if (optlen - 2 > length) {
                /* Bogus - option goes past the end of the header. */
                proto_tree_add_expert_format(opt_tree, pinfo, &ei_tcp_opt_len_invalid, tvb, offset, length,
                                    "%s (option length = %u byte%s says option goes past end of options)",
                                    name, optlen, plurality(optlen, "", "s"));
                return;
            }

            next_tvb = tvb_new_subset_length(tvb, offset, optlen);
            call_dissector_with_data(option_dissector, next_tvb, pinfo, opt_tree/* tree */, data);
            proto_item_append_text(proto_tree_get_parent(opt_tree), ", %s", name);

            offset += optlen;
            length -= (optlen-2); //already accounted for type and len bytes
        }

        if (opt == eol)
            break;
    }
}