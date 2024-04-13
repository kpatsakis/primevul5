R_API void r_io_bank_drain(RIO *io, const ut32 bankid) {
	r_return_if_fail (io);
	RIOBank *bank = r_io_bank_get (io, bankid);
	if (!bank) {
		return;
	}
	bank->last_used = NULL;
	RRBNode *node = r_crbtree_first_node (bank->submaps);
	RRBNode *next = NULL;
	while (node) {
		next = r_rbnode_next (node);
		if (next) {
			RIOSubMap *bd = (RIOSubMap *)node->data;
			RIOSubMap *sm = (RIOSubMap *)next->data;
			if (!memcmp (&bd->mapref, &sm->mapref, sizeof (RIOMapRef))) {
				r_io_submap_set_to (bd, r_io_submap_to (sm));
				r_crbtree_delete (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
				continue;
			}
		}
		node = next;
	}
}