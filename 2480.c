R_API RIOMap *r_io_bank_get_map_at(RIO *io, const ut32 bankid, ut64 addr) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	r_return_val_if_fail (io && bank, NULL);
	RRBNode *node = r_crbtree_find_node (bank->submaps, &addr, _find_sm_by_vaddr_cb, NULL);
	if (!node) {
		return NULL;
	}
	RIOSubMap *sm = (RIOSubMap *)node->data;
	if (!r_io_submap_contain (sm, addr)) {
		return NULL;
	}
	return r_io_map_get_by_ref (io, &sm->mapref);
}