R_API int r_io_bank_write_to_submap_at(RIO *io, const ut32 bankid, ut64 addr, const ut8 *buf, int len) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	r_return_val_if_fail (io && bank, -1);
	if (!len) {
		return 0;
	}
	RRBNode *node;
	if (bank->last_used && r_io_submap_contain (((RIOSubMap *)bank->last_used->data), addr)) {
		node = bank->last_used;
	} else {
		node = r_crbtree_find_node (bank->submaps, &addr, _find_sm_by_vaddr_cb, NULL);
		if (!node) {
			return 0;
		}
		bank->last_used = node;
	}
	RIOSubMap *sm = (RIOSubMap *)node->data;
	if (!r_io_submap_contain (sm, addr)) {
		return 0;
	}
	RIOMap *map = r_io_map_get_by_ref (io, &sm->mapref);
	if (!map || !(map->perm & R_PERM_W)) {
		return -1;
	}
	const int write_len = R_MIN (len, r_io_submap_to (sm) - addr + 1);
	const ut64 paddr = addr - r_io_map_from (map) + map->delta;
	return r_io_fd_write_at (io, map->fd, paddr, buf, write_len);
}