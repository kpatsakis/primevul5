static void snap_all_rawfb(void) {
	int pixelsize = bpp/8;
	int n, sz;
	char *dst;
	static char *unclipped_dst = NULL;
	static int unclipped_len = 0;

	dst = snap->data;

	if (xform24to32 && bpp == 32) {
		pixelsize = 3;
	}
	sz = dpy_y * snap->bytes_per_line;

	if (wdpy_x > dpy_x || wdpy_y > dpy_y) {
		sz = wdpy_x * wdpy_y * pixelsize;
		if (sz > unclipped_len || unclipped_dst == NULL) {
			if (unclipped_dst) {
				free(unclipped_dst);
			}
			unclipped_dst = (char *) malloc(sz+4);
			unclipped_len = sz;
		}
		dst = unclipped_dst;
	}
		
	if (! raw_fb_seek) {
		memcpy(dst, raw_fb_addr + raw_fb_offset, sz);

	} else {
		int len = sz, del = 0;
		off_t off = (off_t) raw_fb_offset;

		lseek(raw_fb_fd, off, SEEK_SET);
		while (len > 0) {
			n = read(raw_fb_fd, dst + del, len);
			if (n > 0) {
				del += n;
				len -= n;
			} else if (n == 0) {
				break;
			} else if (errno != EINTR && errno != EAGAIN) {
				break;
			}
		}
	}

	if (dst == unclipped_dst) {
		char *src;
		int h;
		int x = off_x + coff_x;
		int y = off_y + coff_y;

		src = unclipped_dst + y * wdpy_x * pixelsize +
		    x * pixelsize;
		dst = snap->data;

		for (h = 0; h < dpy_y; h++) {
			memcpy(dst, src, dpy_x * pixelsize);
			src += wdpy_x * pixelsize;
			dst += snap->bytes_per_line;
		}
	}
}