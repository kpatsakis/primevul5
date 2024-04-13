int copy_screen(void) {
	char *fbp;
	int i, y, block_size;

	if (! fs_factor) {
		return 0;
	}
	if (debug_tiles) fprintf(stderr, "copy_screen\n");

	if (unixpw_in_progress) return 0;


	if (! main_fb) {
		return 0;
	}

	block_size = ((dpy_y/fs_factor) * main_bytes_per_line);

	fbp = main_fb;
	y = 0;

	X_LOCK;

	/* screen may be too big for 1 shm area, so broken into fs_factor */
	for (i=0; i < fs_factor; i++) {
		XRANDR_SET_TRAP_RET(-1, "copy_screen-set");
		copy_image(fullscreen, 0, y, 0, 0);
		XRANDR_CHK_TRAP_RET(-1, "copy_screen-chk");

		memcpy(fbp, fullscreen->data, (size_t) block_size);

		y += dpy_y / fs_factor;
		fbp += block_size;
	}

	X_UNLOCK;

	if (blackouts) {
		blackout_regions();
	}

	mark_rect_as_modified(0, 0, dpy_x, dpy_y, 0);
	return 0;
}