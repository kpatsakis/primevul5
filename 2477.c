R_API bool r_io_bank_map_priorize(RIO *io, const ut32 bankid, const ut32 mapid) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	r_return_val_if_fail (io && bank, false);
	RListIter *iter;
	RIOMapRef *mapref;
	r_list_foreach (bank->maprefs, iter, mapref) {
		if (mapref->id == mapid) {
			goto found;
		}
	}
	return false;
found:
	if (iter == bank->maprefs->tail) {	//tail is top
		return r_io_map_get_by_ref (io, mapref) ? true : false;
	}
	RIOSubMap *sm = r_io_submap_new (io, mapref);
	if (!sm) {
		return false;
	}
	RRBNode *entry = _find_entry_submap_node (bank, sm);
	if (!entry) {
		// if this happens, something is really fucked up
		free (sm);
		return false;
	}
	bank->last_used = NULL;
	RIOSubMap *bd = (RIOSubMap *)entry->data;
	if (r_itv_eq (bd->itv, sm->itv)) {
		// no need to insert new sm, if boundaries match perfectly
		// instead override mapref of existing node/submap
		bd->mapref = *mapref;
		free (sm);
		r_list_iter_to_top (bank->maprefs, iter);
		return true;
	}
	if (r_io_submap_from (bd) < r_io_submap_from (sm) &&
		r_io_submap_to (sm) < r_io_submap_to (bd)) {
		// bd completly overlaps sm on both ends,
		// therefor split bd into 2 maps => bd and bdsm
		// |---bd---||--sm--|-bdsm-|
		RIOSubMap *bdsm = R_NEWCOPY (RIOSubMap, bd);
		if (!bdsm) {
			free (sm);
			return false;
		}
		r_io_submap_set_from (bdsm, r_io_submap_to (sm) + 1);
		r_io_submap_set_to (bd, r_io_submap_from (sm) - 1);
		// TODO: insert and check return value, before adjusting sm size
		r_list_iter_to_top (bank->maprefs, iter);
		return r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL) &
			r_crbtree_insert (bank->submaps, bdsm, _find_sm_by_from_vaddr_cb, NULL);
	}

	if (r_io_submap_from (bd) < r_io_submap_from (sm)) {
		r_io_submap_set_to (bd, r_io_submap_from (sm) - 1);
		entry = r_rbnode_next (entry);
	}
	while (entry && r_io_submap_to (((RIOSubMap *)entry->data)) <= r_io_submap_to (sm)) {
		RRBNode *next = r_rbnode_next (entry);
		//delete all submaps that are completly included in sm
		// this can be optimized, there is no need to do search here
		r_crbtree_delete (bank->submaps, entry->data, _find_sm_by_from_vaddr_cb, NULL);
		entry = next;
	}
	if (entry && r_io_submap_from (((RIOSubMap *)entry->data)) <= r_io_submap_to (sm)) {
		bd = (RIOSubMap *)entry->data;
		r_io_submap_set_from (bd, r_io_submap_to (sm) + 1);
	}
	r_list_iter_to_top (bank->maprefs, iter);
	return r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
}