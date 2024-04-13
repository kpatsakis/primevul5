void initialize_tiles(void) {

	ntiles_x = (dpy_x - 1)/tile_x + 1;
	ntiles_y = (dpy_y - 1)/tile_y + 1;
	ntiles = ntiles_x * ntiles_y;

	tile_has_diff = (unsigned char *)
		calloc((size_t) (ntiles * sizeof(unsigned char)), 1);
	tile_has_xdamage_diff = (unsigned char *)
		calloc((size_t) (ntiles * sizeof(unsigned char)), 1);
	tile_row_has_xdamage_diff = (unsigned char *)
		calloc((size_t) (ntiles_y * sizeof(unsigned char)), 1);
	tile_tried    = (unsigned char *)
		calloc((size_t) (ntiles * sizeof(unsigned char)), 1);
	tile_copied   = (unsigned char *)
		calloc((size_t) (ntiles * sizeof(unsigned char)), 1);
	tile_blackout    = (tile_blackout_t *)
		calloc((size_t) (ntiles * sizeof(tile_blackout_t)), 1);
	tile_region = (region_t *) calloc((size_t) (ntiles * sizeof(region_t)), 1);

	tile_row = (XImage **)
		calloc((size_t) ((ntiles_x + 1) * sizeof(XImage *)), 1);
	tile_row_shm = (XShmSegmentInfo *)
		calloc((size_t) ((ntiles_x + 1) * sizeof(XShmSegmentInfo)), 1);

	/* there will never be more hints than tiles: */
	hint_list = (hint_t *) calloc((size_t) (ntiles * sizeof(hint_t)), 1);
}