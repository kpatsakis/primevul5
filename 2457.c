static RRBNode *_find_entry_submap_node(RIOBank *bank, RIOSubMap *sm) {
	RRBNode *node = r_crbtree_find_node (bank->submaps, sm, _find_lowest_intersection_sm_cb, NULL);
	if (!node) {
		return NULL;
	}
	RRBNode *prev = r_rbnode_prev (node);
	while (prev && r_io_submap_overlap (((RIOSubMap *)prev->data), sm)) {
		node = prev;
		prev = r_rbnode_prev (node);
	}
	return node;
}