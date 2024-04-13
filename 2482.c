R_API void r_io_bank_del_map(RIO *io, const ut32 bankid, const ut32 mapid) {
	// no need to check for mapref here, since this is "just" deleting
	RIOBank *bank = r_io_bank_get (io, bankid);
	RIOMap *map = r_io_map_get (io, mapid);	//is this needed?
	r_return_if_fail (bank && map);
	RListIter *iter;
	RIOMapRef *mapref = NULL;
	r_list_foreach_prev (bank->maprefs, iter, mapref) {
		if (mapref->id == map->id) {
			_delete_submaps_from_bank_tree (io, bank, iter, map);
			r_list_delete (bank->maprefs, iter);
			break;
		}
	}
	bank->last_used = NULL;
	// map is not referenced by this bank; nothing to do
}