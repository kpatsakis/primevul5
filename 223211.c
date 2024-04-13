static int island_try(int x, int y, int u, int v, int *run) {
	int n, m, ct;

	n = x + y * ntiles_x;
	m = u + v * ntiles_x;

	if (tile_has_diff[n]) {
		(*run)++;
	} else {
		*run = 0;
	}

	if (tile_has_diff[n] && ! tile_has_diff[m]) {
		/* found a discontinuity */

		if (tile_tried[m]) {
			return 0;
		} else if (*run < grow_fill) {
			return 0;
		}

		ct = copy_tiles(u, v, 1);
		if (ct < 0) return ct;	/* fatal */
	}
	return 1;
}