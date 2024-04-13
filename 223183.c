static int copy_all_tile_runs(void) {
	int x, y, n, m, i;
	int diffs = 0, ct;
	int in_run = 0, run = 0;
	int ntave = 0, ntcnt = 0;

	if (unixpw_in_progress) return 0;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x + 1; x++) {
			n = x + y * ntiles_x;

			if (x != ntiles_x && tile_has_diff[n]) {
				in_run = 1;
				run++;
			} else {
				if (! in_run) {
					in_run = 0;
					run = 0;
					continue;
				}
				ct = copy_tiles(x - run, y, run);
				if (ct < 0) return ct;	/* fatal */

				ntcnt++;
				ntave += run;
				diffs += run;

				/* neighboring tile downward: */
				for (i=1; i <= run; i++) {
					if ((y+1) < ntiles_y
					    && tile_region[n-i].bot_diff) {
						m = (x-i) + (y+1) * ntiles_x;
						if (! tile_has_diff[m]) {
							tile_has_diff[m] = 2;
						}
					}
				}

				/* neighboring tile to right: */
				if (((x-1)+1) < ntiles_x
				    && tile_region[n-1].right_diff) {
					m = ((x-1)+1) + y * ntiles_x;
					if (! tile_has_diff[m]) {
						tile_has_diff[m] = 2;
					}
					
					/* note that this starts a new run */
					in_run = 1;
					run = 1;
				} else {
					in_run = 0;
					run = 0;
				}
			}
		}
		/*
		 * Could some activity go here, to emulate threaded
		 * behavior by servicing some libvncserver tasks?
		 */
	}
	return diffs;
}