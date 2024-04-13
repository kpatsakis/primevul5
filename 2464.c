static void _delete_submaps_from_bank_tree(RIO *io, RIOBank *bank, RListIter *prio, RIOMap *map) {
	RIOSubMap fake_sm;
	fake_sm.itv = map->itv;
	fake_sm.mapref.id = map->id;
	RRBNode *entry = _find_entry_submap_node (bank, &fake_sm);
	if (!entry) {
		return;
	}
	RIOSubMap *bd = (RIOSubMap *)entry->data;
	while (bd && r_io_submap_overlap (bd, (&fake_sm))) {
		// this loop deletes all affected submaps from the rbtree
		// and also enqueues them in bank->todo
		RRBNode *next = r_rbnode_next (entry);
		if (bd->mapref.id == fake_sm.mapref.id) {
			r_queue_enqueue (bank->todo, R_NEWCOPY (RIOSubMap, bd));
			r_crbtree_delete (bank->submaps, bd, _find_sm_by_from_vaddr_cb, NULL);
		}
		entry = next;
		bd = entry ? (RIOSubMap *)entry->data : NULL;
	}
	RListIter *iter = prio;
	while (!r_queue_is_empty (bank->todo)) {
		// now check for each deleted submap if a lower map intersects with it
		// and create new submaps accordingly, and fill the gaps
		RIOSubMap *sm = r_queue_dequeue (bank->todo);
		RListIter *ator = r_list_iter_get_prev (iter);
		while (ator) {
			map = r_io_map_get_by_ref (io, (RIOMapRef *)ator->data);
			ator = r_list_iter_get_prev (ator);
			if (!map) {
				// if this happens, something is fucked up, and no submap should be inserted
				continue;
			}
			// if the map and sm intersect, the intersecting submap needs to be inserted in the tree
			// there are 5 cases to consider here
			// 1. no intersection: just continue to the next iteration
			// 2. map overlaps sm on both ends: insert submap for map with boundaries of sm
			// 3. map overlaps sm on the upper end: insert submap for map accordingly and adjust sm boundaries
			// 4. map overlaps sm on the lower end: insert submap for map accordingly and adjust sm boundaries
			// 5. sm overlaps sm on both ends: split sm into 2 submaps and enqueue new one in banks->todo; insert submap for map; adjust sm boundaries
			if (r_io_submap_to (sm) < r_io_map_from (map) || r_io_submap_from (sm) > r_io_map_to (map)) {
				// case 1
				continue;
			}
			RIOMapRef *mapref = _mapref_from_map (map);
			bd = r_io_submap_new (io, mapref);
			free (mapref);
			if (!bd) {
				continue;
			}
			if (r_io_submap_from (sm) >= r_io_map_from (map)) {
				// case 4 and 2
				r_io_submap_set_from (bd, r_io_submap_from (sm));
				r_crbtree_insert (bank->submaps, bd, _find_sm_by_from_vaddr_cb, NULL);
				if (r_io_submap_to (sm) <= r_io_map_to (map)) {
					// case 2
					r_io_submap_set_to (bd, r_io_submap_to (sm));
					break;
				}
				// case 4
				r_io_submap_set_from (sm, r_io_submap_to (bd) + 1);
				continue;
			}
			if (r_io_submap_to (sm) <= r_io_map_to (map)) {
				// case 3
				// adjust bd upper boundary to avoid overlap with existing submaps
				r_io_submap_set_to (bd, r_io_submap_to (sm));
				// adjust sm upper boundary to avoid hitting again on sm in further iterations
				r_io_submap_set_to (sm, r_io_submap_from (bd) - 1);
				r_crbtree_insert (bank->submaps, bd, _find_sm_by_from_vaddr_cb, NULL);
				continue;
			}
			// case 5 because all other cases are already handled
			RIOSubMap *bdsm = R_NEWCOPY (RIOSubMap, sm);
			r_io_submap_set_to (sm, r_io_submap_from (bd) - 1);
			r_io_submap_set_from (bdsm, r_io_submap_to (bd) + 1);
			r_crbtree_insert (bank->submaps, bd, _find_sm_by_from_vaddr_cb, NULL);
			r_queue_enqueue (bank->todo, bdsm);
		}
		free (sm);
	}
}