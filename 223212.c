static void nap_set(int tile_cnt) {
	int nap_in = nap_ok;
	time_t now = time(NULL);

	if (scan_count == 0) {
		/* roll up check for all NSCAN scans */
		nap_ok = 0;
		if (naptile && nap_diff_count < 2 * NSCAN * naptile) {
			/* "2" is a fudge to permit a bit of bg drawing */
			nap_ok = 1;
		}
		nap_diff_count = 0;
	}
	if (nap_ok && ! nap_in && use_xdamage) {
		if (XD_skip > 0.8 * XD_tot) 	{
			/* X DAMAGE is keeping load low, so skip nap */
			nap_ok = 0;
		}
	}
	if (! nap_ok && client_count) {
		if(now > last_fb_bytes_sent + no_fbu_blank) {
			if (debug_tiles > 1) {
				fprintf(stderr, "nap_set: nap_ok=1: now: %d last: %d\n",
				    (int) now, (int) last_fb_bytes_sent);
			}
			nap_ok = 1;
		}
	}

	if (show_cursor) {
		/* kludge for the up to 4 tiles the mouse patch could occupy */
		if ( tile_cnt > 4) {
			last_event = now;
		}
	} else if (tile_cnt != 0) {
		last_event = now;
	}
}