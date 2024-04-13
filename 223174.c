static int copy_tiles_additional_pass(void) {
	int x, y, n;
	int diffs = 0, ct;

	if (unixpw_in_progress) return 0;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x; x++) {
			n = x + y * ntiles_x;		/* number of this tile */

			if (! tile_has_diff[n]) {
				continue;
			}
			if (tile_copied[n]) {
				continue;
			}

			ct = copy_tiles(x, y, 1);
			if (ct < 0) return ct;	/* fatal */
		}
	}
	for (n=0; n < ntiles; n++) {
		if (tile_has_diff[n]) {
			diffs++;
		}
	}
	return diffs;
}