static int _mapref_priority_cmp(RIOBank *bank, RIOMapRef *mr0, RIOMapRef *mr1) {
	if (mr0->id == mr1->id) {
		// mapref have the same priority, if their mapid matches
		return 0;
	}
	RListIter *iter;
	RIOMapRef *mapref;
	r_list_foreach_prev (bank->maprefs, iter, mapref) {
		if (mapref->id == mr0->id) {
			return 1;
		}
		if (mapref->id == mr1->id) {
			return -1;
		}
	}
	return 0;	// should never happen
}