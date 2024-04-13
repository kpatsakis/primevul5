static void save_hint(hint_t hint, int loc) {
	/* simply copy it to the global array for later use. */
	hint_list[loc].x = hint.x;
	hint_list[loc].y = hint.y;
	hint_list[loc].w = hint.w;
	hint_list[loc].h = hint.h;
}