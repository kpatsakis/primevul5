void initialize_polling_images(void) {
	int i, MB = 1024 * 1024;

	/* set all shm areas to "none" before trying to create any */
	scanline_shm.shmid	= -1;
	scanline_shm.shmaddr	= (char *) -1;
	scanline		= NULL;
	fullscreen_shm.shmid	= -1;
	fullscreen_shm.shmaddr	= (char *) -1;
	fullscreen		= NULL;
	snaprect_shm.shmid	= -1;
	snaprect_shm.shmaddr	= (char *) -1;
	snaprect		= NULL;
	for (i=1; i<=ntiles_x; i++) {
		tile_row_shm[i].shmid	= -1;
		tile_row_shm[i].shmaddr	= (char *) -1;
		tile_row[i]		= NULL;
	}

	/* the scanline (e.g. 1280x1) shared memory area image: */

	if (! shm_create(&scanline_shm, &scanline, dpy_x, 1, "scanline")) {
		clean_up_exit(1);
	}

	/*
	 * the fullscreen (e.g. 1280x1024/fs_factor) shared memory area image:
	 * (we cut down the size of the shm area to try avoid and shm segment
	 * limits, e.g. the default 1MB on Solaris)
	 */
	if (UT.sysname && strstr(UT.sysname, "Linux")) {
		set_fs_factor(10 * MB);
	} else {
		set_fs_factor(1 * MB);
	}
	if (fs_frac >= 1.0) {
		fs_frac = 1.1;
		fs_factor = 0;
	}
	if (! fs_factor) {
		rfbLog("warning: fullscreen updates are disabled.\n");
	} else {
		if (! shm_create(&fullscreen_shm, &fullscreen, dpy_x,
		    dpy_y/fs_factor, "fullscreen")) {
			clean_up_exit(1);
		}
	}
	if (use_snapfb) {
		if (! fs_factor) {
			rfbLog("warning: disabling -snapfb mode.\n");
			use_snapfb = 0;
		} else if (! shm_create(&snaprect_shm, &snaprect, dpy_x,
		    dpy_y/fs_factor, "snaprect")) {
			clean_up_exit(1);
		}
	}

	/*
	 * for copy_tiles we need a lot of shared memory areas, one for
	 * each possible run length of changed tiles.  32 for 1024x768
	 * and 40 for 1280x1024, etc. 
	 */

	tile_shm_count = 0;
	for (i=1; i<=ntiles_x; i++) {
		if (! shm_create(&tile_row_shm[i], &tile_row[i], tile_x * i,
		    tile_y, "tile_row")) {
			if (i == 1) {
				clean_up_exit(1);
			}
			rfbLog("shm: Error creating shared memory tile-row for"
			    " len=%d,\n", i);
			rfbLog("shm: reverting to -onetile mode. If this"
			    " problem persists\n");
			rfbLog("shm: try using the -onetile or -noshm options"
			    " to limit\n");
			rfbLog("shm: shared memory usage, or run ipcrm(1)"
			    " to manually\n");
			rfbLog("shm: delete unattached shm segments.\n");
			single_copytile_count = i;
			single_copytile = 1;
		}
		tile_shm_count++;
		if (single_copytile && i >= 1) {
			/* only need 1x1 tiles */
			break;
		}
	}
	if (verbose) {
		if (using_shm && ! xform24to32) {
			rfbLog("created %d tile_row shm polling images.\n",
			    tile_shm_count);
		} else {
			rfbLog("created %d tile_row polling images.\n",
			    tile_shm_count);
		}
	}
}