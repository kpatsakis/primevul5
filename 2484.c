R_API bool r_io_bank_map_depriorize(RIO *io, const ut32 bankid, const ut32 mapid) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	RIOMap *map = r_io_map_get (io, mapid);
	r_return_val_if_fail (bank && map, false);
	RListIter *iter;
	RIOMapRef *mapref = NULL;
	r_list_foreach (bank->maprefs, iter, mapref) {
		if (mapref->id == mapid) {
			goto found;
		}
	}
	// map is not referenced by this bank
	return false;
found:
	if (iter == bank->maprefs->head) {
		// map is already lowest priority
		return true;
	}
	bank->last_used = NULL;
	_delete_submaps_from_bank_tree (io, bank, iter, map);
	r_list_delete (bank->maprefs, iter);
	return r_io_bank_map_add_bottom (io, bankid, mapid);
}