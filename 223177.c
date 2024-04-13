int scan_for_updates(int count_only) {
	int i, tile_count, tile_diffs;
	int old_copy_tile;
	double frac1 = 0.1;   /* tweak parameter to try a 2nd scan_display() */
	double frac2 = 0.35;  /* or 3rd */
	double frac3 = 0.02;  /* do scan_display() again after copy_tiles() */
	static double last_poll = 0.0;

	if (unixpw_in_progress) return 0;
 
	if (slow_fb > 0.0) {
		double now = dnow();
		if (now < last_poll + slow_fb) {
			return 0;
		}
		last_poll = now;
	}

	for (i=0; i < ntiles; i++) {
		tile_has_diff[i] = 0;
		tile_has_xdamage_diff[i] = 0;
		tile_tried[i] = 0;
		tile_copied[i] = 0;
	}
	for (i=0; i < ntiles_y; i++) {
		/* could be useful, currently not used */
		tile_row_has_xdamage_diff[i] = 0;
	}
	xdamage_tile_count = 0;

	/*
	 * n.b. this program has only been tested so far with
	 * tile_x = tile_y = NSCAN = 32!
	 */

	if (!count_only) {
		scan_count++;
		scan_count %= NSCAN;

		/* some periodic maintenance */
		if (subwin && scan_count % 4 == 0) {
			set_offset();	/* follow the subwindow */
		}
		if (indexed_color && scan_count % 4 == 0) {
			/* check for changed colormap */
			set_colormap(0);
		}
		if (cmap8to24 && scan_count % 1 == 0) {
			check_for_multivis();
		}
#ifdef MACOSX
		if (macosx_console) {
			macosx_event_loop();
		}
#endif
		if (use_xdamage) {
			/* first pass collecting DAMAGE events: */
#ifdef MACOSX
			if (macosx_console) {
				collect_non_X_xdamage(-1, -1, -1, -1, 0);
			} else 
#endif
			{
				if (rawfb_vnc_reflect) {
					collect_non_X_xdamage(-1, -1, -1, -1, 0);
				} else {
					collect_xdamage(scan_count, 0);
				}
			}
		}
	}

#define SCAN_FATAL(x) \
	if (x < 0) { \
		scan_in_progress = 0; \
		fb_copy_in_progress = 0; \
		return 0; \
	}

	/* scan with the initial y to the jitter value from scanlines: */
	scan_in_progress = 1;
	tile_count = scan_display(scanlines[scan_count], 0);
	SCAN_FATAL(tile_count);

	/*
	 * we do the XDAMAGE here too since after scan_display()
	 * there is a better chance we have received the events from
	 * the X server (otherwise the DAMAGE events will be processed
	 * in the *next* call, usually too late and wasteful since
	 * the unchanged tiles are read in again).
	 */
	if (use_xdamage) {
#ifdef MACOSX
		if (macosx_console) {
			;
		} else 
#endif
		{
			if (rawfb_vnc_reflect) {
				;
			} else {
				collect_xdamage(scan_count, 1);
			}
		}
	}
	if (count_only) {
		scan_in_progress = 0;
		fb_copy_in_progress = 0;
		return tile_count;
	}

	if (xdamage_tile_count) {
		/* pick up "known" damaged tiles we missed in scan_display() */
		for (i=0; i < ntiles; i++) {
			if (tile_has_diff[i]) {
				continue;
			}
			if (tile_has_xdamage_diff[i]) {
				tile_has_diff[i] = 1;
				if (tile_has_xdamage_diff[i] == 1) {
					tile_has_xdamage_diff[i] = 2;
					tile_count++;
				}
			}
		}
	}
	if (dpy && use_xdamage == 1) {
		static time_t last_xd_check = 0;
		if (time(NULL) > last_xd_check + 2) {
			int cp = (scan_count + 3) % NSCAN;
			xd_do_check = 1;
			tile_count = scan_display(scanlines[cp], 0);
			xd_do_check = 0;
			SCAN_FATAL(tile_count);
			last_xd_check = time(NULL);
			if (xd_samples > 200) {
				static int bad = 0;
				if (xd_misses > (20 * xd_samples) / 100) {
					rfbLog("XDAMAGE is not working well... misses: %d/%d\n", xd_misses, xd_samples);
					rfbLog("Maybe an OpenGL app like Beryl or Compiz is the problem?\n");
					rfbLog("Use x11vnc -noxdamage or disable the Beryl/Compiz app.\n");
					rfbLog("To disable this check and warning specify -xdamage twice.\n");
					if (++bad >= 10) {
						rfbLog("XDAMAGE appears broken (OpenGL app?), turning it off.\n");
						use_xdamage = 0;
						initialize_xdamage();
						destroy_xdamage_if_needed();
					}
				}
				xd_samples = 0;
				xd_misses = 0;
			}
		}
	}

	nap_set(tile_count);

	if (fs_factor && frac1 >= fs_frac) {
		/* make frac1 < fs_frac if fullscreen updates are enabled */
		frac1 = fs_frac/2.0;
	}

	if (tile_count > frac1 * ntiles) {
		/*
		 * many tiles have changed, so try a rescan (since it should
		 * be short compared to the many upcoming copy_tiles() calls)
		 */

		/* this check is done to skip the extra scan_display() call */
		if (! fs_factor || tile_count <= fs_frac * ntiles) {
			int cp, tile_count_old = tile_count;
			
			/* choose a different y shift for the 2nd scan: */
			cp = (NSCAN - scan_count) % NSCAN;

			tile_count = scan_display(scanlines[cp], 1);
			SCAN_FATAL(tile_count);

			if (tile_count >= (1 + frac2) * tile_count_old) {
				/* on a roll... do a 3rd scan */
				cp = (NSCAN - scan_count + 7) % NSCAN;
				tile_count = scan_display(scanlines[cp], 1);
				SCAN_FATAL(tile_count);
			}
		}
		scan_in_progress = 0;

		/*
		 * At some number of changed tiles it is better to just
		 * copy the full screen at once.  I.e. time = c1 + m * r1
		 * where m is number of tiles, r1 is the copy_tiles()
		 * time, and c1 is the scan_display() time: for some m
		 * it crosses the full screen update time.
		 *
		 * We try to predict that crossover with the fs_frac
		 * fudge factor... seems to be about 1/2 the total number
		 * of tiles.  n.b. this ignores network bandwidth,
		 * compression time etc...
		 *
		 * Use -fs 1.0 to disable on slow links.
		 */
		if (fs_factor && tile_count > fs_frac * ntiles) {
			int cs;
			fb_copy_in_progress = 1;
			cs = copy_screen();
			fb_copy_in_progress = 0;
			SCAN_FATAL(cs);
			if (use_threads && pointer_mode != 1) {
				pointer_event(-1, 0, 0, NULL);
			}
			nap_check(tile_count);
			return tile_count;
		}
	}
	scan_in_progress = 0;

	/* copy all tiles with differences from display to rfb framebuffer: */
	fb_copy_in_progress = 1;

	if (single_copytile || tile_shm_count < ntiles_x) {
		/*
		 * Old way, copy I/O one tile at a time.
		 */
		old_copy_tile = 1;
	} else {
		/* 
		 * New way, does runs of horizontal tiles at once.
		 * Note that below, for simplicity, the extra tile finding
		 * (e.g. copy_tiles_backward_pass) is done the old way.
		 */
		old_copy_tile = 0;
	}

	if (unixpw_in_progress) return 0;

	if (old_copy_tile) {
		tile_diffs = copy_all_tiles();
	} else {
		tile_diffs = copy_all_tile_runs();
	}
	SCAN_FATAL(tile_diffs);

	/*
	 * This backward pass for upward and left tiles complements what
	 * was done in copy_all_tiles() for downward and right tiles.
	 */
	tile_diffs = copy_tiles_backward_pass();
	SCAN_FATAL(tile_diffs);

	if (tile_diffs > frac3 * ntiles) {
		/*
		 * we spent a lot of time in those copy_tiles, run
		 * another scan, maybe more of the screen changed.
		 */
		int cp = (NSCAN - scan_count + 13) % NSCAN;

		scan_in_progress = 1;
		tile_count = scan_display(scanlines[cp], 1);
		SCAN_FATAL(tile_count);
		scan_in_progress = 0;

		tile_diffs = copy_tiles_additional_pass();
		SCAN_FATAL(tile_diffs);
	}

	/* Given enough tile diffs, try the islands: */
	if (grow_fill && tile_diffs > 4) {
		tile_diffs = grow_islands();
	}
	SCAN_FATAL(tile_diffs);

	/* Given enough tile diffs, try the gaps: */
	if (gaps_fill && tile_diffs > 4) {
		tile_diffs = fill_tile_gaps();
	}
	SCAN_FATAL(tile_diffs);

	fb_copy_in_progress = 0;
	if (use_threads && pointer_mode != 1) {
		/*
		 * tell the pointer handler it can process any queued
		 * pointer events:
		 */
		pointer_event(-1, 0, 0, NULL);
	}

	if (blackouts) {
		/* ignore any diffs in completely covered tiles */
		int x, y, n;
		for (y=0; y < ntiles_y; y++) {
			for (x=0; x < ntiles_x; x++) {
				n = x + y * ntiles_x;
				if (tile_blackout[n].cover == 2) {
					tile_has_diff[n] = 0;
				}
			}
		}
	}

	hint_updates();	/* use x0rfbserver hints algorithm */

	/* Work around threaded rfbProcessClientMessage() calls timeouts */
	if (use_threads) {
		ping_clients(tile_diffs);
	} else if (saw_ultra_chat || saw_ultra_file) {
		ping_clients(-1);
	} else if (use_openssl && !tile_diffs) {
		ping_clients(0);
	}
	/* -ping option: */
	if (ping_interval) {
		int td = ping_interval > 0 ? ping_interval : -ping_interval;
		ping_clients(-td);
	}


	nap_check(tile_diffs);
	return tile_diffs;
}