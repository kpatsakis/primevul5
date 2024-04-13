R_API bool r_io_bank_update_map_boundaries(RIO *io, const ut32 bankid, const ut32 mapid, ut64 ofrom, ut64 oto) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	r_return_val_if_fail (io && bank, false);
	RListIter *iter;
	RIOMapRef *mapref;
	r_list_foreach_prev (bank->maprefs, iter, mapref) {
		if (mapref->id == mapid) {
			goto found;
		}
	}
	// map is not referenced by this map
	return false;
found:
	;RIOMap *map = r_io_map_get_by_ref (io, mapref);
	if (!map) {
		// inconsistent mapref
		// mapref should be deleted from bank here
		return false;
	}
	if (r_io_map_from (map) == ofrom && r_io_map_to (map) == oto) {
		// nothing todo here
		return true;
	}
	// allocate sm here to avoid deleting things without ensuring
	// that this code could at least insert 1 submap
	RIOSubMap *sm = r_io_submap_new (io, mapref);
	if (!sm) {
		return false;
	}

	bank->last_used = NULL;
	// this problem can be divided in 2 steps:
	// 1. delete corresponding submaps and insert intersecting submaps with lower priority
	// 2. adjust addr and insert submaps at new addr respecting priority
	RIOMap fake_map;
	memcpy (&fake_map, map, sizeof (RIOMap));
	fake_map.itv.addr = ofrom;
	fake_map.itv.size = oto - ofrom + 1;
	_delete_submaps_from_bank_tree (io, bank, iter, &fake_map);

	RRBNode *entry = _find_entry_submap_node (bank, sm);
	if (!entry) {
		// no intersection here, so just insert sm into the tree and we're done
		r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
		// assumption here is that there is no need to check for return value of r_crbtree_insert,
		// since it only fails, if allocation fails and a delete was performed before, so it should just be fine
		return true;
	}

	RIOSubMap *bd = (RIOSubMap *)entry->data;
	// check if sm has higher priority than bd by comparing their maprefs
	if (_mapref_priority_cmp (bank, &sm->mapref, &bd->mapref) == 1) {
		// sm has higher priority that bd => adjust bd
		if (r_io_submap_to (bd) == r_io_submap_to (sm)) {
			if (r_io_submap_from (bd) >= r_io_submap_from (sm)) {
				// bc of _find_entry_submap_node, we can be sure, that there is no
				// lower submap that intersects with sm
				//
				// instead of deleting and inserting, just replace the mapref,
				// similar to r_io_bank_map_priorize
				memcpy (bd, sm, sizeof (RIOSubMap));
				free (sm);
			} else {
				r_io_submap_set_to (bd, r_io_submap_from (sm) - 1);
				r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
			}
			return true;
		}
		if (r_io_submap_from (bd) < r_io_submap_from (sm) &&
			r_io_submap_to (sm) < r_io_submap_to (bd)) {
			RIOSubMap *bdsm = R_NEWCOPY (RIOSubMap, bd);
			// allocating bdsm here is fine, bc bd is already in the tree
			r_io_submap_set_from (bdsm, r_io_submap_to (sm) + 1);
			r_io_submap_set_to (bd, r_io_submap_from (sm) - 1);
			// What do if this fails?
			r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
			r_crbtree_insert (bank->submaps, bdsm, _find_sm_by_from_vaddr_cb, NULL);
			return true;
		}
		if (r_io_submap_from (bd) < r_io_submap_from (sm)) {
			r_io_submap_set_to (bd, r_io_submap_from (sm) - 1);
			entry = r_rbnode_next (entry);
		}
	} else {
		// _mapref_priority_cmp cannot return 0 in this scenario,
		// since all submaps with the same mapref as sm were deleted from
		// the submap tree previously. so _mapref_priority_cmp can only return 1 or -1
		// bd has higher priority than sm => adjust sm
		if (r_io_submap_from (bd) <= r_io_submap_from (sm)) {
			if (r_io_submap_to (sm) <= r_io_submap_to (bd)) {
				// bd completly overlaps sm => nothing to do
				free (sm);
				return true;
			} // else
			// adjust sm
			// r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
		} else {
			if (r_io_submap_to (sm) <= r_io_submap_to (bd)) {
				r_io_submap_set_to (sm, r_io_submap_from (bd) - 1);
				if (!r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL)) {
					free (sm);
					return false;
				}
				return true;
			}
			RIOSubMap *bdsm = R_NEWCOPY (RIOSubMap, sm);
			if (!bdsm) {
				free (sm);
				return false;
			}
			r_io_submap_set_to (bdsm, r_io_submap_from (bd) - 1);
			// r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
			if (!r_crbtree_insert (bank->submaps, bdsm, _find_sm_by_from_vaddr_cb, NULL)) {
				free (bdsm);
				free (sm);
				return false;
			}
			// r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
			entry = r_rbnode_next (entry);
		}
		r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
	}
	// entry = r_rbnode_next (entry);
	// it is given that entry->data->from >= sm->from on every iteration
	// so only check for upper boundary of sm for intersection with entry->data
	while (entry && r_io_submap_to (((RIOSubMap *)entry->data)) <= r_io_submap_to (sm)) {
		// iterate forwards starting at entry, while entry->data and sm overlap
		bd = (RIOSubMap *)entry->data;
		entry = r_rbnode_next (entry);
		// check if sm has higher priority than bd by comparing their maprefs
		if (_mapref_priority_cmp (bank, &sm->mapref, &bd->mapref) == 1) {
			// delete bd
			r_crbtree_delete (bank->submaps, bd, _find_sm_by_from_vaddr_cb, NULL);
		} else {
			// _mapref_priority_cmp cannot return 0 in this scenario,
			// since all submaps with the same mapref as sm were deleted from
			// the submap tree previously. so _mapref_priority_cmp can only return 1 or -1
			// bd has higher priority than sm => adjust sm
			if (r_io_submap_from (bd) > r_io_submap_from (sm)) {
				RIOSubMap *bdsm = R_NEWCOPY (RIOSubMap, sm);
				r_io_submap_set_to (bdsm, r_io_submap_from (bd) - 1);
				r_crbtree_insert (bank->submaps, bdsm, _find_sm_by_from_vaddr_cb, NULL);
			}
			if (r_io_submap_to (bd) == r_io_submap_to (sm)) {
				// in this case the size of sm would be 0,
				// but since empty maps are not allowed free sm and return
				free (sm);
				return true;
			}
			r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
		}
	}
	if (!entry) {
		return r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
	}
	bd = (RIOSubMap *)entry->data;
	if (_mapref_priority_cmp (bank, &sm->mapref, &bd->mapref) == 1) {
		if (r_io_submap_from (bd) <= r_io_submap_to (sm)) {
			r_io_submap_set_from (bd, r_io_submap_to (sm) + 1);
		}
		r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
	} else {
		if (r_io_submap_from (sm) < r_io_submap_from (bd)) {
			if (r_io_submap_from (bd) <= r_io_submap_to (sm)) {
				r_io_submap_set_to (sm, r_io_submap_from (bd) - 1);
			}
			r_crbtree_insert (bank->submaps, sm, _find_sm_by_from_vaddr_cb, NULL);
		} else {
			// can this happen?
			free (sm);
		}
	}
	return true;
}