static int _find_sm_by_from_vaddr_cb(void *incoming, void *in, void *user) {
	RIOSubMap *bd = (RIOSubMap *)incoming, *sm = (RIOSubMap *)in;
	if (r_io_submap_from (bd) < r_io_submap_from (sm)) {
		return -1;
	}
	if (r_io_submap_from (bd) > r_io_submap_from (sm)) {
		return 1;
	}
	return 0;
}