mptcp_add_duplicated_dsn(packet_info *pinfo _U_, proto_tree *tree, tvbuff_t *tvb, struct mptcp_subflow *subflow,
guint64 rawdsn64low, guint64 rawdsn64high
)
{
    wmem_list_t *results = NULL;
    wmem_list_frame_t *packet_it = NULL;
    mptcp_dsn2packet_mapping_t *packet = NULL;
    proto_item *item = NULL;

    results = wmem_itree_find_intervals(subflow->dsn2packet_map,
                    wmem_packet_scope(),
                    rawdsn64low,
                    rawdsn64high
                    );

    for(packet_it = wmem_list_head(results);
        packet_it != NULL;
        packet_it = wmem_list_frame_next(packet_it))
    {

        packet = (mptcp_dsn2packet_mapping_t *) wmem_list_frame_data(packet_it);
        DISSECTOR_ASSERT(packet);

        if(pinfo->num > packet->frame) {
            item = proto_tree_add_uint(tree, hf_mptcp_reinjection_of, tvb, 0, 0, packet->frame);
        }
        else {
            item = proto_tree_add_uint(tree, hf_mptcp_reinjected_in, tvb, 0, 0, packet->frame);
        }
        PROTO_ITEM_SET_GENERATED(item);
    }

    return packet;
}