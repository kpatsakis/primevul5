R_API bool r_io_bank_locate(RIO *io, const ut32 bankid, ut64 *addr, const ut64 size, ut64 load_align) {
	RIOBank *bank = r_io_bank_get (io, bankid);
	r_return_val_if_fail (io && bank && bank->submaps && addr && size, false);
	if (load_align == 0LL) {
		load_align = 1;
	}
	RIOSubMap fake_sm;
	memset (&fake_sm, 0x00, sizeof(RIOSubMap));
	fake_sm.itv.addr = *addr + (load_align - *addr % load_align) % load_align;
	fake_sm.itv.size = size;
	RRBNode *entry = _find_entry_submap_node (bank, &fake_sm);
	if (!entry) {
		// no submaps in this bank
		*addr = fake_sm.itv.addr;
		return true;
	}
	// this is a bit meh: first iteration can never be successful,
	// bc entry->sm will always intersect with fake_sm, if
	// _find_entry_submap_node suceeded previously
	ut64 next_location = fake_sm.itv.addr;
	while (entry) {
		RIOSubMap *sm = (RIOSubMap *)entry->data;
		if (size <= r_io_submap_from (sm) - next_location) {
			*addr = next_location;
			return true;
		}
		next_location = (r_io_submap_to (sm) + 1) +
			(load_align - ((r_io_submap_to (sm) + 1) % load_align)) % load_align;
		entry = r_rbnode_next (entry);
	}
	if (next_location == 0LL) {
		// overflow from last submap in the tree => no location
		return false;
	}
	if (UT64_MAX - size + 1 < next_location) {
		return false;
	}
	*addr = next_location;
	return true;
}