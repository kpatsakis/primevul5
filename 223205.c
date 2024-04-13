static int blackout_line_skip(int n, int x, int y, int rescan,
    int *tile_count) {
	
	if (tile_blackout[n].cover == 2) {
		tile_has_diff[n] = 0;
		return 1;	/* skip it */

	} else if (tile_blackout[n].cover == 1) {
		int w, x1, y1, x2, y2, b, hit = 0;
		if (x + NSCAN > dpy_x) {
			w = dpy_x - x;
		} else {
			w = NSCAN;
		}

		for (b=0; b < tile_blackout[n].count; b++) {
			
			/* n.b. these coords are in full display space: */
			x1 = tile_blackout[n].bo[b].x1;
			x2 = tile_blackout[n].bo[b].x2;
			y1 = tile_blackout[n].bo[b].y1;
			y2 = tile_blackout[n].bo[b].y2;

			if (x2 - x1 < w) {
				/* need to cover full width */
				continue;
			}
			if (y1 <= y && y < y2) {
				hit = 1;
				break;
			}
		}
		if (hit) {
			if (! rescan) {
				tile_has_diff[n] = 0;
			} else {
				*tile_count += tile_has_diff[n];
			}
			return 1;	/* skip */
		}
	}
	return 0;	/* do not skip */
}