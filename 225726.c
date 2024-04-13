tcp_print_timestamps(packet_info *pinfo, tvbuff_t *tvb, proto_tree *parent_tree, struct tcp_analysis *tcpd, struct tcp_per_packet_data_t *tcppd)
{
    proto_item  *item;
    proto_tree  *tree;
    nstime_t    ts;

    if (!tcpd)
        return;

    tree=proto_tree_add_subtree(parent_tree, tvb, 0, 0, ett_tcp_timestamps, &item, "Timestamps");
    PROTO_ITEM_SET_GENERATED(item);

    nstime_delta(&ts, &pinfo->abs_ts, &tcpd->ts_first);
    item = proto_tree_add_time(tree, hf_tcp_ts_relative, tvb, 0, 0, &ts);
    PROTO_ITEM_SET_GENERATED(item);

    if( !tcppd )
        tcppd = (struct tcp_per_packet_data_t *)p_get_proto_data(wmem_file_scope(), pinfo, proto_tcp, pinfo->curr_layer_num);

    if( tcppd ) {
        item = proto_tree_add_time(tree, hf_tcp_ts_delta, tvb, 0, 0,
            &tcppd->ts_del);
        PROTO_ITEM_SET_GENERATED(item);
    }
}