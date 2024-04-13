static void snap_vcsa_rawfb(void) {
	int n;
	char *dst;
	char buf[32];
	int i, len, del;
	unsigned char rows, cols, xpos, ypos;
	static int prev_rows = -1, prev_cols = -1;
	static unsigned char prev_xpos = -1, prev_ypos = -1;
	static char *vcsabuf  = NULL;
	static char *vcsabuf0 = NULL;
	static unsigned int color_tab[16];
	static int Cw = 8, Ch = 16;
	static int db = -1, first = 1;
	int created = 0;
	rfbScreenInfo s;
	rfbScreenInfoPtr fake_screen = &s;
	int Bpp = raw_fb_native_bpp / 8;

	if (db < 0) {
		if (getenv("X11VNC_DEBUG_VCSA")) {
			db = atoi(getenv("X11VNC_DEBUG_VCSA"));
		} else {
			db = 0;
		}
	}

	if (first) {
		unsigned int rm = raw_fb_native_red_mask;
		unsigned int gm = raw_fb_native_green_mask;
		unsigned int bm = raw_fb_native_blue_mask;
		unsigned int rs = raw_fb_native_red_shift;
		unsigned int gs = raw_fb_native_green_shift;
		unsigned int bs = raw_fb_native_blue_shift;
		unsigned int rx = raw_fb_native_red_max;
		unsigned int gx = raw_fb_native_green_max;
		unsigned int bx = raw_fb_native_blue_max;

		for (i=0; i < 16; i++) {
			int r = console_cmap[3*i+0];
			int g = console_cmap[3*i+1];
			int b = console_cmap[3*i+2];
			r = rx * r / 255;
			g = gx * g / 255;
			b = bx * b / 255;
			color_tab[i] = (r << rs) | (g << gs) | (b << bs);
			if (db) fprintf(stderr, "cmap[%02d] 0x%08x  %04d %04d %04d\n", i, color_tab[i], r, g, b); 
			if (i != 0 && getenv("RAWFB_VCSA_BW")) {
				color_tab[i] = rm | gm | bm;
			}
		}
	}
	first = 0;

	lseek(raw_fb_fd, 0, SEEK_SET);
	len = 4;
	del = 0;
	memset(buf, 0, sizeof(buf));
	while (len > 0) {
		n = read(raw_fb_fd, buf + del, len);
		if (n > 0) {
			del += n;
			len -= n;
		} else if (n == 0) {
			break;
		} else if (errno != EINTR && errno != EAGAIN) {
			break;
		}
	}

	rows = (unsigned char) buf[0];
	cols = (unsigned char) buf[1];
	xpos = (unsigned char) buf[2];
	ypos = (unsigned char) buf[3];

	if (db) fprintf(stderr, "rows=%d cols=%d xpos=%d ypos=%d Bpp=%d\n", rows, cols, xpos, ypos, Bpp);
	if (rows == 0 || cols == 0) {
		usleep(100 * 1000);
		return;
	}

	if (vcsabuf == NULL || prev_rows != rows || prev_cols != cols) {
		if (vcsabuf) {
			free(vcsabuf);
			free(vcsabuf0);
		}
		vcsabuf  = (char *) calloc(2 * rows * cols, 1);
		vcsabuf0 = (char *) calloc(2 * rows * cols, 1);
		created = 1;

		if (prev_rows != -1 && prev_cols != -1) {
			do_new_fb(1);
		}

		prev_rows = rows;
		prev_cols = cols;
	}

	if (!rfbEndianTest) {
		unsigned char tc = rows;
		rows = cols;
		cols = tc;

		tc = xpos;
		xpos = ypos;
		ypos = tc;
	}

	len = 2 * rows * cols;
	del = 0;
	memset(vcsabuf, 0, len);
	while (len > 0) {
		n = read(raw_fb_fd, vcsabuf + del, len);
		if (n > 0) {
			del += n;
			len -= n;
		} else if (n == 0) {
			break;
		} else if (errno != EINTR && errno != EAGAIN) {
			break;
		}
	}

	fake_screen->frameBuffer = snap->data;
	fake_screen->paddedWidthInBytes = snap->bytes_per_line;
	fake_screen->serverFormat.bitsPerPixel = raw_fb_native_bpp;
	fake_screen->width = snap->width;
	fake_screen->height = snap->height;

	for (i=0; i < rows * cols; i++) {
		int ix, iy, x, y, w, h;
		unsigned char chr = 0;
		unsigned char attr;
		unsigned int fore, back;
		unsigned short *usp;
		unsigned int *uip;
		chr  = (unsigned char) vcsabuf[2*i];
		attr = vcsabuf[2*i+1];

		iy = i / cols;
		ix = i - iy * cols;

		if (ix == prev_xpos && iy == prev_ypos) {
			;
		} else if (ix == xpos && iy == ypos) {
			;
		} else if (!created && chr == vcsabuf0[2*i] && attr == vcsabuf0[2*i+1]) {
			continue;
		}

		if (!rfbEndianTest) {
			unsigned char tc = chr;
			chr = attr;
			attr = tc;
		}

		y = iy * Ch;
		x = ix * Cw;
		dst = snap->data + y * snap->bytes_per_line + x * Bpp;

		fore = color_tab[attr & 0xf];
		back = color_tab[(attr >> 4) & 0x7];

		if (ix == xpos && iy == ypos) {
			unsigned int ti = fore;
			fore = back;
			back = ti;
		}

		for (h = 0; h < Ch; h++) {
			if (Bpp == 1) {
				memset(dst, back, Cw);
			} else if (Bpp == 2) {
				for (w = 0; w < Cw; w++) {
					usp = (unsigned short *) (dst + w*Bpp); 
					*usp = (unsigned short) back;
				}
			} else if (Bpp == 4) {
				for (w = 0; w < Cw; w++) {
					uip = (unsigned int *) (dst + w*Bpp); 
					*uip = (unsigned int) back;
				}
			}
			dst += snap->bytes_per_line;
		}
		rfbDrawChar(fake_screen, &default8x16Font, x, y + Ch, chr, fore);
	}
	memcpy(vcsabuf0, vcsabuf, 2 * rows * cols); 
	prev_xpos = xpos;
	prev_ypos = ypos;
}