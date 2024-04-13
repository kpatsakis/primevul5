mptcp_add_analysis_subtree(packet_info *pinfo, tvbuff_t *tvb, proto_tree *parent_tree,
                          struct tcp_analysis *tcpd, struct mptcp_analysis *mptcpd, struct tcpheader * tcph)
{

    proto_item *item = NULL;
    proto_tree *tree = NULL;
    mptcp_per_packet_data_t *mptcppd = NULL;

    if(mptcpd == NULL) {
        return;
    }

    item=proto_tree_add_item(parent_tree, hf_mptcp_analysis, tvb, 0, 0, ENC_NA);
    PROTO_ITEM_SET_GENERATED(item);
    tree=proto_item_add_subtree(item, ett_mptcp_analysis);
    PROTO_ITEM_SET_GENERATED(tree);

    /* set field with mptcp stream */
    if(mptcpd->master) {

        item = proto_tree_add_boolean_format_value(tree, hf_mptcp_analysis_master, tvb, 0,
                                     0, (mptcpd->master->stream == tcpd->stream) ? TRUE : FALSE
                                     , "Master is tcp stream %u", mptcpd->master->stream
                                     );

    }
    else {
          item = proto_tree_add_boolean(tree, hf_mptcp_analysis_master, tvb, 0,
                                     0, FALSE);
    }

    PROTO_ITEM_SET_GENERATED(item);

    item = proto_tree_add_uint(tree, hf_mptcp_stream, tvb, 0, 0, mptcpd->stream);
    PROTO_ITEM_SET_GENERATED(item);

    /* retrieve saved analysis of packets, else create it */
    mptcppd = (mptcp_per_packet_data_t *)p_get_proto_data(wmem_file_scope(), pinfo, proto_mptcp, pinfo->curr_layer_num);
    if(!mptcppd) {
        mptcppd = (mptcp_per_packet_data_t *)wmem_new0(wmem_file_scope(), mptcp_per_packet_data_t);
        p_add_proto_data(wmem_file_scope(), pinfo, proto_mptcp, pinfo->curr_layer_num, mptcppd);
    }

    /* Print formatted list of tcp stream ids that are part of the connection */
    mptcp_analysis_add_subflows(pinfo, tvb, tree, mptcpd);

    /* Converts TCP seq number into its MPTCP DSN */
    mptcp_analysis_dsn_lookup(pinfo, tvb, tree, tcpd, tcph, mptcppd);

}