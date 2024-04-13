static int copy_tiles(int tx, int ty, int nt) {
	int x, y, line;
	int size_x, size_y, width1, width2;
	int off, len, n, dw, dx, t;
	int w1, w2, dx1, dx2;	/* tmps for normal and short tiles */
	int pixelsize = bpp/8;
	int first_min, last_max;
	int first_x = -1, last_x = -1;
	static int prev_ntiles_x = -1;

	char *src, *dst, *s_src, *s_dst, *m_src, *m_dst;
	char *h_src, *h_dst;
	if (unixpw_in_progress) return 0;

	if (ntiles_x != prev_ntiles_x && first_line != NULL) {
		free(first_line);	first_line = NULL;
		free(last_line);	last_line = NULL;
		free(left_diff);	left_diff = NULL;
		free(right_diff);	right_diff = NULL;
	}

	if (first_line == NULL) {
		/* allocate arrays first time in. */
		int n = ntiles_x + 1;
		rfbLog("copy_tiles: allocating first_line at size %d\n", n);
		first_line = (int *) malloc((size_t) (n * sizeof(int)));
		last_line  = (int *) malloc((size_t) (n * sizeof(int)));
		left_diff  = (unsigned short *)
			malloc((size_t) (n * sizeof(unsigned short)));
		right_diff = (unsigned short *)
			malloc((size_t) (n * sizeof(unsigned short)));
	}
	prev_ntiles_x = ntiles_x;

	x = tx * tile_x;
	y = ty * tile_y;

	size_x = dpy_x - x;
	if ( size_x > tile_x * nt ) {
		size_x = tile_x * nt;
		width1 = tile_x;
		width2 = tile_x;
	} else {
		/* short tile */
		width1 = tile_x;	/* internal tile */
		width2 = size_x - (nt - 1) * tile_x;	/* right hand tile */
	}

	size_y = dpy_y - y;
	if ( size_y > tile_y ) {
		size_y = tile_y;
	}

	n = tx + ty * ntiles_x;		/* number of the first tile */

	if (blackouts && tile_blackout[n].cover == 2) {
		/*
		 * If there are blackouts and this tile is completely covered
		 * no need to poll screen or do anything else..
		 * n.b. we are in single copy_tile mode: nt=1
		 */
		tile_has_diff[n] = 0;
		return(0);
	}

	X_LOCK;
	XRANDR_SET_TRAP_RET(-1, "copy_tile-set");
	/* read in the whole tile run at once: */
	copy_image(tile_row[nt], x, y, size_x, size_y);
	XRANDR_CHK_TRAP_RET(-1, "copy_tile-chk");


	X_UNLOCK;

	if (blackouts && tile_blackout[n].cover == 1) {
		/*
		 * If there are blackouts and this tile is partially covered
		 * we should re-black-out the portion.
		 * n.b. we are in single copy_tile mode: nt=1
		 */
		int x1, x2, y1, y2, b;
		int w, s, fill = 0;

		for (b=0; b < tile_blackout[n].count; b++) {
			char *b_dst = tile_row[nt]->data;
			
			x1 = tile_blackout[n].bo[b].x1 - x;
			y1 = tile_blackout[n].bo[b].y1 - y;
			x2 = tile_blackout[n].bo[b].x2 - x;
			y2 = tile_blackout[n].bo[b].y2 - y;

			w = (x2 - x1) * pixelsize;
			s = x1 * pixelsize;

			for (line = 0; line < size_y; line++) {
				if (y1 <= line && line < y2) {
					memset(b_dst + s, fill, (size_t) w);
				}
				b_dst += tile_row[nt]->bytes_per_line;
			}
		}
	}

	src = tile_row[nt]->data;
	dst = main_fb + y * main_bytes_per_line + x * pixelsize;

	s_src = src;
	s_dst = dst;

	for (t=1; t <= nt; t++) {
		first_line[t] = -1;
	}

	/* find the first line with difference: */
	w1 = width1 * pixelsize;
	w2 = width2 * pixelsize;

	/* foreach line: */
	for (line = 0; line < size_y; line++) {
		/* foreach horizontal tile: */
		for (t=1; t <= nt; t++) {
			if (first_line[t] != -1) {
				continue;
			}

			off = (t-1) * w1;
			if (t == nt) {
				len = w2;	/* possible short tile */
			} else {
				len = w1;
			}
			
			if (memcmp(s_dst + off, s_src + off, len)) {
				first_line[t] = line;
			}
		}
		s_src += tile_row[nt]->bytes_per_line;
		s_dst += main_bytes_per_line;
	}

	/* see if there were any differences for any tile: */
	first_min = -1;
	for (t=1; t <= nt; t++) {
		tile_tried[n+(t-1)] = 1;
		if (first_line[t] != -1) {
			if (first_min == -1 || first_line[t] < first_min) {
				first_min = first_line[t];
			}
		}
	}
	if (first_min == -1) {
		/* no tile has a difference, note this and get out: */
		for (t=1; t <= nt; t++) {
			tile_has_diff[n+(t-1)] = 0;
		}
		return(0);
	} else {
		/*
		 * at least one tile has a difference.  make sure info
		 * is recorded (e.g. sometimes we guess tiles and they
		 * came in with tile_has_diff 0)
		 */
		for (t=1; t <= nt; t++) {
			if (first_line[t] == -1) {
				tile_has_diff[n+(t-1)] = 0;
			} else {
				tile_has_diff[n+(t-1)] = 1;
			}
		}
	}

	m_src = src + (tile_row[nt]->bytes_per_line * size_y);
	m_dst = dst + (main_bytes_per_line * size_y);

	for (t=1; t <= nt; t++) {
		last_line[t] = first_line[t];
	}

	/* find the last line with difference: */
	w1 = width1 * pixelsize;
	w2 = width2 * pixelsize;

	/* foreach line: */
	for (line = size_y - 1; line > first_min; line--) {

		m_src -= tile_row[nt]->bytes_per_line;
		m_dst -= main_bytes_per_line;

		/* foreach tile: */
		for (t=1; t <= nt; t++) {
			if (first_line[t] == -1
			    || last_line[t] != first_line[t]) {
				/* tile has no changes or already done */
				continue;
			}

			off = (t-1) * w1;
			if (t == nt) {
				len = w2;	/* possible short tile */
			} else {
				len = w1;
			}
			if (memcmp(m_dst + off, m_src + off, len)) {
				last_line[t] = line;
			}
		}
	}
	
	/*
	 * determine the farthest down last changed line
	 * will be used below to limit our memcpy() to the framebuffer.
	 */
	last_max = -1;
	for (t=1; t <= nt; t++) {
		if (first_line[t] == -1) {
			continue;
		}
		if (last_max == -1 || last_line[t] > last_max) {
			last_max = last_line[t];
		}
	}

	/* look for differences on left and right hand edges: */
	for (t=1; t <= nt; t++) {
		left_diff[t] = 0;
		right_diff[t] = 0;
	}

	h_src = src;
	h_dst = dst;

	w1 = width1 * pixelsize;
	w2 = width2 * pixelsize;

	dx1 = (width1 - tile_fuzz) * pixelsize;
	dx2 = (width2 - tile_fuzz) * pixelsize;
	dw = tile_fuzz * pixelsize; 

	/* foreach line: */
	for (line = 0; line < size_y; line++) {
		/* foreach tile: */
		for (t=1; t <= nt; t++) {
			if (first_line[t] == -1) {
				/* tile has no changes at all */
				continue;
			}

			off = (t-1) * w1;
			if (t == nt) {
				dx = dx2;	/* possible short tile */
				if (dx <= 0) {
					break;
				}
			} else {
				dx = dx1;
			}

			if (! left_diff[t] && memcmp(h_dst + off,
			    h_src + off, dw)) {
				left_diff[t] = 1;
			}
			if (! right_diff[t] && memcmp(h_dst + off + dx,
			    h_src + off + dx, dw) ) {
				right_diff[t] = 1;
			}
		}
		h_src += tile_row[nt]->bytes_per_line;
		h_dst += main_bytes_per_line;
	}

	/* now finally copy the difference to the rfb framebuffer: */
	s_src = src + tile_row[nt]->bytes_per_line * first_min;
	s_dst = dst + main_bytes_per_line * first_min;

	for (line = first_min; line <= last_max; line++) {
		/* for I/O speed we do not do this tile by tile */
		memcpy(s_dst, s_src, size_x * pixelsize);
		if (nt == 1) {
			/*
			 * optimization for tall skinny lines, e.g. wm
			 * frame. try to find first_x and last_x to limit
			 * the size of the hint.  could help for a slow
			 * link.  Unfortunately we spent a lot of time
			 * reading in the many tiles.
			 *
			 * BTW, we like to think the above memcpy leaves
			 * the data we use below in the cache... (but
			 * it could be two 128 byte segments at 32bpp)
			 * so this inner loop is not as bad as it seems.
			 */
			int k, kx;
			kx = pixelsize;
			for (k=0; k<size_x; k++) {
				if (memcmp(s_dst + k*kx, s_src + k*kx, kx))  {
					if (first_x == -1 || k < first_x) {
						first_x = k;
					}
					if (last_x == -1 || k > last_x) {
						last_x = k;
					}
				}
			}
		}
		s_src += tile_row[nt]->bytes_per_line;
		s_dst += main_bytes_per_line;
	}

	/* record all the info in the region array for this tile: */
	for (t=1; t <= nt; t++) {
		int s = t - 1;

		if (first_line[t] == -1) {
			/* tile unchanged */
			continue;
		}
		tile_region[n+s].first_line = first_line[t];
		tile_region[n+s].last_line  = last_line[t];

		tile_region[n+s].first_x = first_x;
		tile_region[n+s].last_x  = last_x;

		tile_region[n+s].top_diff = 0;
		tile_region[n+s].bot_diff = 0;
		if ( first_line[t] < tile_fuzz ) {
			tile_region[n+s].top_diff = 1;
		}
		if ( last_line[t] > (size_y - 1) - tile_fuzz ) {
			tile_region[n+s].bot_diff = 1;
		}

		tile_region[n+s].left_diff  = left_diff[t];
		tile_region[n+s].right_diff = right_diff[t];

		tile_copied[n+s] = 1;
	}

	return(1);
}