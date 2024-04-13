int copy_snap(void) {
	int db = 1;
	char *fbp;
	int i, y, block_size;
	double dt;
	static int first = 1, snapcnt = 0;

	if (raw_fb_str) {
		int read_all_at_once = 1;
		double start = dnow();
		if (rawfb_reset < 0) {
			if (getenv("SNAPFB_RAWFB_RESET")) {
				rawfb_reset = 1;
			} else {
				rawfb_reset = 0;
			}
		}
		if (snap_fb == NULL || snap == NULL) {
			rfbLog("copy_snap: rawfb mode and null snap fb\n"); 
			clean_up_exit(1);
		}
		if (rawfb_reset) {
			initialize_raw_fb(1);
		}
		if (raw_fb_bytes_per_line != snap->bytes_per_line) {
			read_all_at_once = 0;
		}
		if (raw_fb_full_str && strstr(raw_fb_full_str, "/dev/vcsa")) {
			snap_vcsa_rawfb();
		} else if (read_all_at_once) {
			snap_all_rawfb();
		} else {
			/* this goes line by line, XXX not working for video */
			copy_raw_fb(snap, 0, 0, dpy_x, dpy_y);
		}
if (db && snapcnt++ < 5) rfbLog("rawfb copy_snap took: %.5f secs\n", dnow() - start);

		return 0;
	}
	
	if (! fs_factor) {
		return 0;
	}


	if (! snap_fb || ! snap || ! snaprect) {
		return 0;
	}
	block_size = ((dpy_y/fs_factor) * snap->bytes_per_line);

	fbp = snap_fb;
	y = 0;


	dtime0(&dt);
	X_LOCK;

	/* screen may be too big for 1 shm area, so broken into fs_factor */
	for (i=0; i < fs_factor; i++) {
		XRANDR_SET_TRAP_RET(-1, "copy_snap-set");
		copy_image(snaprect, 0, y, 0, 0);
		XRANDR_CHK_TRAP_RET(-1, "copy_snap-chk");

		memcpy(fbp, snaprect->data, (size_t) block_size);

		y += dpy_y / fs_factor;
		fbp += block_size;
	}

	X_UNLOCK;

	dt = dtime(&dt);
	if (first) {
		rfbLog("copy_snap: time for -snapfb snapshot: %.3f sec\n", dt);
		first = 0;
	}

	return 0;
}