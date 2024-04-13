void mark_wrapper(int x1, int y1, int x2, int y2) {
	int t, r_x1 = x1, r_y1 = y1, r_x2 = x2, r_y2 = y2;

	if (rotating) {
		/* well we hope rot_fb will always be the last one... */
		rotate_coords(x1, y1, &r_x1, &r_y1, -1, -1);
		rotate_coords(x2, y2, &r_x2, &r_y2, -1, -1);
		rotate_fb(x1, y1, x2, y2);
		if (r_x1 > r_x2) {
			t = r_x1;
			r_x1 = r_x2;
			r_x2 = t;
		}
		if (r_y1 > r_y2) {
			t = r_y1;
			r_y1 = r_y2;
			r_y2 = t;
		}
		/* painting errors  */
		r_x1--;
		r_x2++;
		r_y1--;
		r_y2++;
	}
	rfbMarkRectAsModified(screen, r_x1, r_y1, r_x2, r_y2);
}