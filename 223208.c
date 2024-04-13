static void hint_updates(void) {
	hint_t hint;
	int x, y, i, n, ty, th, tx, tw;
	int hint_count = 0, in_run = 0;

	hint.x = hint.y = hint.w = hint.h = 0;

	for (y=0; y < ntiles_y; y++) {
		for (x=0; x < ntiles_x; x++) {
			n = x + y * ntiles_x;

			if (tile_has_diff[n]) {
				ty = tile_region[n].first_line;
				th = tile_region[n].last_line - ty + 1;

				tx = tile_region[n].first_x;
				tw = tile_region[n].last_x - tx + 1;
				if (tx < 0) {
					tx = 0;
					tw = tile_x;
				}

				if (! in_run) {
					create_tile_hint( x * tile_x + tx,
					    y * tile_y + ty, tw, th, &hint);
					in_run = 1;
				} else {
					extend_tile_hint( x * tile_x + tx,
					    y * tile_y + ty, tw, th, &hint);
				}
			} else {
				if (in_run) {
					/* end of a row run of altered tiles: */
					save_hint(hint, hint_count++);
					in_run = 0;
				}
			}
		}
		if (in_run) {	/* save the last row run */
			save_hint(hint, hint_count++);
			in_run = 0;
		}
	}

	for (i=0; i < hint_count; i++) {
		/* pass update info to vnc: */
		mark_hint(hint_list[i]);
	}
}