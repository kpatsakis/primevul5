mptcp_analysis_add_subflows(packet_info *pinfo _U_,  tvbuff_t *tvb,
    proto_tree *parent_tree, struct mptcp_analysis* mptcpd)
{
    wmem_list_frame_t *it;
    proto_tree *tree;
    proto_item *item;

    item=proto_tree_add_item(parent_tree, hf_mptcp_analysis_subflows, tvb, 0, 0, ENC_NA);
    PROTO_ITEM_SET_GENERATED(item);

    tree=proto_item_add_subtree(item, ett_mptcp_analysis_subflows);

    /* for the analysis, we set each subflow tcp stream id */
    for(it = wmem_list_head(mptcpd->subflows); it != NULL; it = wmem_list_frame_next(it)) {
        struct tcp_analysis *sf = (struct tcp_analysis *)wmem_list_frame_data(it);
        proto_item *subflow_item;
        subflow_item=proto_tree_add_uint(tree, hf_mptcp_analysis_subflows_stream_id, tvb, 0, 0, sf->stream);
        PROTO_ITEM_SET_HIDDEN(subflow_item);

        proto_item_append_text(item, " %d", sf->stream);
    }

    PROTO_ITEM_SET_GENERATED(item);
}