R_API bool r_io_bank_read_at(RIO *io, const ut32 bankid, ut64 addr, ut8 *buf, int len) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	r_return_val_if_fail (io && bank, false);
	RIOSubMap fake_sm;
	memset (&fake_sm, 0x00, sizeof(RIOSubMap));
	fake_sm.itv.addr = addr;
	fake_sm.itv.size = len;
	RRBNode *node;
	if (bank->last_used && r_io_submap_contain (((RIOSubMap *)bank->last_used->data), addr)) {
		node = bank->last_used;
	} else {
		node = _find_entry_submap_node (bank, &fake_sm);
	}
	memset (buf, io->Oxff, len);
	RIOSubMap *sm = node ? (RIOSubMap *)node->data : NULL;
	bool ret = true;
	while (sm && r_io_submap_overlap ((&fake_sm), sm)) {
		bank->last_used = node;
		RIOMap *map = r_io_map_get_by_ref (io, &sm->mapref);
		if (!map) {
			// mapref doesn't belong to map
			return false;
		}
		if (!(map->perm & R_PERM_R)) {
			node = r_rbnode_next (node);
			sm = node ? (RIOSubMap *)node->data : NULL;
			continue;
		}
		const ut64 buf_off = R_MAX (addr, r_io_submap_from (sm)) - addr;
		const int read_len = R_MIN (r_io_submap_to ((&fake_sm)),
					     r_io_submap_to (sm)) - (addr + buf_off) + 1;
		const ut64 paddr = addr + buf_off - r_io_map_from (map) + map->delta;
		ret &= (r_io_fd_read_at (io, map->fd, paddr, &buf[buf_off], read_len) == read_len);
		// check return value here?
		node = r_rbnode_next (node);
		sm = node ? (RIOSubMap *)node->data : NULL;
	}
	return ret;
}