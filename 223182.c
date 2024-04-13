static int scan_display(int ystart, int rescan) {
	char *src, *dst;
	int pixelsize = bpp/8;
	int x, y, w, n;
	int tile_count = 0;
	int nodiffs = 0, diff_hint;
	int xd_check = 0, xd_freq = 1;
	static int xd_tck = 0;

	y = ystart;

	g_now = dnow();

	if (! main_fb) {
		rfbLog("scan_display: no main_fb!\n");
		return 0;
	}

	X_LOCK;

	while (y < dpy_y) {

		if (use_xdamage) {
			XD_tot++;
			xd_check = 0;
			if (xdamage_hint_skip(y)) {
				if (xd_do_check && dpy && use_xdamage == 1) {
					xd_tck++;
					xd_tck = xd_tck % xd_freq;
					if (xd_tck == 0) {
						xd_check = 1;
						xd_samples++;
					}
				}
				if (!xd_check) {
					XD_skip++;
					y += NSCAN;
					continue;
				}
			} else {
				if (xd_do_check && 0) {
					fprintf(stderr, "ns y=%d\n", y);
				}
			}
		}

		/* grab the horizontal scanline from the display: */

#ifndef NO_NCACHE
/* XXX Y test */
if (ncache > 0) {
	int gotone = 0;
	if (macosx_console) {
		if (macosx_checkevent(NULL)) {
			gotone = 1;
		}
	} else {
#if !NO_X11
		XEvent ev;
		if (raw_fb_str) {
			;
		} else if (XEventsQueued(dpy, QueuedAlready) == 0) {
			;	/* XXX Y resp */
		} else if (XCheckTypedEvent(dpy, MapNotify, &ev)) {
			gotone = 1;
		} else if (XCheckTypedEvent(dpy, UnmapNotify, &ev)) {
			gotone = 2;
		} else if (XCheckTypedEvent(dpy, CreateNotify, &ev)) {
			gotone = 3;
		} else if (XCheckTypedEvent(dpy, ConfigureNotify, &ev)) {
			gotone = 4;
		} else if (XCheckTypedEvent(dpy, VisibilityNotify, &ev)) {
			gotone = 5;
		}
		if (gotone) {
			XPutBackEvent(dpy, &ev);
		}
#endif
	}
	if (gotone) {
		static int nomsg = 1;
		if (nomsg) {
			if (dnowx() > 20) {
				nomsg = 0;
			}
		} else {
if (ncdb) fprintf(stderr, "\n*** SCAN_DISPLAY CHECK_NCACHE/%d *** %d rescan=%d\n", gotone, y, rescan);
		}
		X_UNLOCK;
		check_ncache(0, 1);
		X_LOCK;
	}
}
#endif

		XRANDR_SET_TRAP_RET(-1, "scan_display-set");
		copy_image(scanline, 0, y, 0, 0);
		XRANDR_CHK_TRAP_RET(-1, "scan_display-chk");

		/* for better memory i/o try the whole line at once */
		src = scanline->data;
		dst = main_fb + y * main_bytes_per_line;

		if (! memcmp(dst, src, main_bytes_per_line)) {
			/* no changes anywhere in scan line */
			nodiffs = 1;
			if (! rescan) {
				y += NSCAN;
				continue;
			}
		}
		if (xd_check) {
			xd_misses++;
		}

		x = 0;
		while (x < dpy_x) {
			n = (x/tile_x) + (y/tile_y) * ntiles_x;
			diff_hint = 0;

			if (blackouts) {
				if (blackout_line_skip(n, x, y, rescan,
				    &tile_count)) {
					x += NSCAN;
					continue;
				}
			}

			if (rescan) {
				if (nodiffs || tile_has_diff[n]) {
					tile_count += tile_has_diff[n];
					x += NSCAN;
					continue;
				}
			} else if (xdamage_tile_count &&
			    tile_has_xdamage_diff[n]) {
				tile_has_xdamage_diff[n] = 2;
				diff_hint = 1;
			}

			/* set ptrs to correspond to the x offset: */
			src = scanline->data + x * pixelsize;
			dst = main_fb + y * main_bytes_per_line + x * pixelsize;

			/* compute the width of data to be compared: */
			if (x + NSCAN > dpy_x) {
				w = dpy_x - x;
			} else {
				w = NSCAN;
			}

			if (diff_hint || memcmp(dst, src, w * pixelsize)) {
				/* found a difference, record it: */
				if (! blackouts) {
					tile_has_diff[n] = 1;
					tile_count++;		
				} else {
					if (blackout_line_cmpskip(n, x, y,
					    dst, src, w, pixelsize)) {
						tile_has_diff[n] = 0;
					} else {
						tile_has_diff[n] = 1;
						tile_count++;		
					}
				}
			}
			x += NSCAN;
		}
		y += NSCAN;
	}

	X_UNLOCK;

	return tile_count;
}