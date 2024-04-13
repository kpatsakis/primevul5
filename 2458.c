R_API bool r_io_bank_map_add_bottom(RIO *io, const ut32 bankid, const ut32 mapid) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	RIOMap *map = r_io_map_get (io, mapid);
	r_return_val_if_fail (io && bank && map, false);
	RIOMapRef *mapref = _mapref_from_map (map);
	if (!mapref) {
		return false;
	}
	RIOSubMap *sm = r_io_submap_new (io, mapref);
	if (!sm) {
		free (mapref);
		return false;
	}
	RRBNode *entry = _find_entry_submap_node (bank, sm);
	if (!entry) {
		// no intersection with any submap, so just insert
		if (!r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL)) {
			free (sm);
			free (mapref);
			return false;
		}
		r_list_prepend (bank->maprefs, mapref);
		return true;
	}
	while (entry && r_io_submap_from (((RIOSubMap *)entry->data)) <= r_io_submap_to (sm)) {
		RIOSubMap *bd = (RIOSubMap *)entry->data;
		if (r_io_submap_from (sm) < r_io_submap_from (bd)) {
			RIOSubMap *bdsm = R_NEWCOPY (RIOSubMap, sm);
			r_io_submap_set_to (bdsm, r_io_submap_from (bd) - 1);
			r_crbtree_insert (bank->submaps, bdsm, _find_sm_by_from_vaddr_cb, NULL);
		}
		if (r_io_submap_to (sm) <= r_io_submap_to (bd)) {
			r_list_prepend (bank->maprefs, mapref);
			free (sm);
			return true;
		}
		r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
		entry = r_rbnode_next (entry);
	}
	r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
	r_list_prepend (bank->maprefs, mapref);
	return true;
}