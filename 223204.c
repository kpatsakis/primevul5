static void print_tiles(void) {
	/* hack for viewing tile diffs on the screen. */
	static char *prev = NULL;
	int n, x, y, ms = 1500;

	ms = 1;

	if (! prev) {
		prev = (char *) malloc((size_t) ntiles);
		for (n=0; n < ntiles; n++) {
			prev[n] = 0;
		}
	}
	fprintf(stderr, "   ");
	for (x=0; x < ntiles_x; x++) {
		fprintf(stderr, "%1d", x % 10);
	}
	fprintf(stderr, "\n");
	n = 0;
	for (y=0; y < ntiles_y; y++) {
		fprintf(stderr, "%2d ", y);
		for (x=0; x < ntiles_x; x++) {
			if (tile_has_diff[n]) {
				fprintf(stderr, "X");
			} else if (prev[n]) {
				fprintf(stderr, "o");
			} else {
				fprintf(stderr, ".");
			}
			n++;
		}
		fprintf(stderr, "\n");
	}
	for (n=0; n < ntiles; n++) {
		prev[n] = tile_has_diff[n];
	}
	usleep(ms * 1000);
}