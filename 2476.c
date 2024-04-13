static int _find_sm_by_vaddr_cb(void *incoming, void *in, void *user) {
	const ut64 addr = ((ut64 *)incoming)[0];
	RIOSubMap *sm = (RIOSubMap *)in;
	if (r_io_submap_contain (sm, addr)) {
		return 0;
	}
	if (addr < r_io_submap_from (sm)) {
		return -1;
	}
	return 1;
}