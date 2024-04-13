static int blackout_line_cmpskip(int n, int x, int y, char *dst, char *src,
    int w, int pixelsize) {

	int i, x1, y1, x2, y2, b, hit = 0;
	int beg = -1, end = -1; 

	if (tile_blackout[n].cover == 0) {
		return 0;	/* 0 means do not skip it. */
	} else if (tile_blackout[n].cover == 2) {
		return 1;	/* 1 means skip it. */
	}

	/* tile has partial coverage: */

	for (i=0; i < w * pixelsize; i++)  {
		if (*(dst+i) != *(src+i)) {
			beg = i/pixelsize;	/* beginning difference */
			break;
		}
	}
	for (i = w * pixelsize - 1; i >= 0; i--)  {
		if (*(dst+i) != *(src+i)) {
			end = i/pixelsize;	/* ending difference */
			break;
		}
	}
	if (beg < 0 || end < 0) {
		/* problem finding range... */
		return 0;
	}

	/* loop over blackout rectangles: */
	for (b=0; b < tile_blackout[n].count; b++) {
		
		/* y in full display space: */
		y1 = tile_blackout[n].bo[b].y1;
		y2 = tile_blackout[n].bo[b].y2;

		/* x relative to tile origin: */
		x1 = tile_blackout[n].bo[b].x1 - x;
		x2 = tile_blackout[n].bo[b].x2 - x;

		if (y1 > y || y >= y2) {
			continue;
		}
		if (x1 <= beg && end <= x2) {
			hit = 1;
			break;
		}
	}
	if (hit) {
		return 1;
	} else {
		return 0;
	}
}