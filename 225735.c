tcp_sequence_number_analysis_print_duplicate(packet_info * pinfo,
                          tvbuff_t * tvb,
                          proto_tree * flags_tree,
                          struct tcp_acked *ta,
                          proto_tree * tree
                        )
{
    proto_item * flags_item;

    /* TCP Duplicate ACK */
    if (ta->dupack_num) {
        if (ta->flags & TCP_A_DUPLICATE_ACK ) {
            flags_item=proto_tree_add_none_format(flags_tree,
                                                  hf_tcp_analysis_duplicate_ack,
                                                  tvb, 0, 0,
                                                  "This is a TCP duplicate ack"
                );
            PROTO_ITEM_SET_GENERATED(flags_item);
            col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                                   "[TCP Dup ACK %u#%u] ",
                                   ta->dupack_frame,
                                   ta->dupack_num
                );

        }
        flags_item=proto_tree_add_uint(tree, hf_tcp_analysis_duplicate_ack_num,
                                       tvb, 0, 0, ta->dupack_num);
        PROTO_ITEM_SET_GENERATED(flags_item);
        flags_item=proto_tree_add_uint(tree, hf_tcp_analysis_duplicate_ack_frame,
                                       tvb, 0, 0, ta->dupack_frame);
        PROTO_ITEM_SET_GENERATED(flags_item);
        expert_add_info_format(pinfo, flags_item, &ei_tcp_analysis_duplicate_ack, "Duplicate ACK (#%u)", ta->dupack_num);
    }
}