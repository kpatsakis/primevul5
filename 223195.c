void free_tiles(void) {
	if (tile_has_diff) {
		free(tile_has_diff);
		tile_has_diff = NULL;
	}
	if (tile_has_xdamage_diff) {
		free(tile_has_xdamage_diff);
		tile_has_xdamage_diff = NULL;
	}
	if (tile_row_has_xdamage_diff) {
		free(tile_row_has_xdamage_diff);
		tile_row_has_xdamage_diff = NULL;
	}
	if (tile_tried) {
		free(tile_tried);
		tile_tried = NULL;
	}
	if (tile_copied) {
		free(tile_copied);
		tile_copied = NULL;
	}
	if (tile_blackout) {
		free(tile_blackout);
		tile_blackout = NULL;
	}
	if (tile_region) {
		free(tile_region);
		tile_region = NULL;
	}
	if (tile_row) {
		free(tile_row);
		tile_row = NULL;
	}
	if (tile_row_shm) {
		free(tile_row_shm);
		tile_row_shm = NULL;
	}
	if (hint_list) {
		free(hint_list);
		hint_list = NULL;
	}
}