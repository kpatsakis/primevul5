static void blackout_regions(void) {
	int i;
	for (i=0; i < blackouts; i++) {
		zero_fb(blackr[i].x1, blackr[i].y1, blackr[i].x2, blackr[i].y2);
	}
}