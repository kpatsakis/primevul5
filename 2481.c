static int _find_lowest_intersection_sm_cb(void *incoming, void *in, void *user) {
	RIOSubMap *bd = (RIOSubMap *)incoming, *sm = (RIOSubMap *)in;
	if (r_io_submap_overlap (bd, sm)) {
		return 0;
	}
	if (r_io_submap_from (bd) < r_io_submap_from (sm)) {
		return -1;
	}
	return 1;
}