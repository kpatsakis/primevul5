static int fill_tile_gaps(void) {
	int x, y, run, saw;
	int n, diffs = 0, ct;

	/* horizontal: */
	for (y=0; y < ntiles_y; y++) {
		run = 0;
		saw = 0;
		for (x=0; x < ntiles_x; x++) {
			ct = gap_try(x, y, &run, &saw, 1);
			if (ct < 0) return ct;	/* fatal */
		}
	}

	/* vertical: */
	for (x=0; x < ntiles_x; x++) {
		run = 0;
		saw = 0;
		for (y=0; y < ntiles_y; y++) {
			ct = gap_try(x, y, &run, &saw, 0);
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