print_tcp_fragment_tree(fragment_head *ipfd_head, proto_tree *tree, proto_tree *tcp_tree, packet_info *pinfo, tvbuff_t *next_tvb)
{
    proto_item *tcp_tree_item, *frag_tree_item;

    /*
     * The subdissector thought it was completely
     * desegmented (although the stuff at the
     * end may, in turn, require desegmentation),
     * so we show a tree with all segments.
     */
    show_fragment_tree(ipfd_head, &tcp_segment_items,
        tree, pinfo, next_tvb, &frag_tree_item);
    /*
     * The toplevel fragment subtree is now
     * behind all desegmented data; move it
     * right behind the TCP tree.
     */
    tcp_tree_item = proto_tree_get_parent(tcp_tree);
    if(frag_tree_item && tcp_tree_item) {
        proto_tree_move_item(tree, tcp_tree_item, frag_tree_item);
    }
}