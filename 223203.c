void set_offset(void) {
	Window w;
	if (! subwin) {
		return;
	}
	X_LOCK;
	xtranslate(window, rootwin, 0, 0, &off_x, &off_y, &w, 0);
	X_UNLOCK;
}