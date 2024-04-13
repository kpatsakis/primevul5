static void vt_console_print(struct console *co, const char *b, unsigned count)
{
	struct vc_data *vc = vc_cons[fg_console].d;
	unsigned char c;
	static DEFINE_SPINLOCK(printing_lock);
	const ushort *start;
	ushort start_x, cnt;
	int kmsg_console;

	/* console busy or not yet initialized */
	if (!printable)
		return;
	if (!spin_trylock(&printing_lock))
		return;

	kmsg_console = vt_get_kmsg_redirect();
	if (kmsg_console && vc_cons_allocated(kmsg_console - 1))
		vc = vc_cons[kmsg_console - 1].d;

	if (!vc_cons_allocated(fg_console)) {
		/* impossible */
		/* printk("vt_console_print: tty %d not allocated ??\n", currcons+1); */
		goto quit;
	}

	if (vc->vc_mode != KD_TEXT)
		goto quit;

	/* undraw cursor first */
	if (con_is_fg(vc))
		hide_cursor(vc);

	start = (ushort *)vc->vc_pos;
	start_x = vc->vc_x;
	cnt = 0;
	while (count--) {
		c = *b++;
		if (c == 10 || c == 13 || c == 8 || vc->vc_need_wrap) {
			if (cnt && con_is_visible(vc))
				vc->vc_sw->con_putcs(vc, start, cnt, vc->vc_y, start_x);
			cnt = 0;
			if (c == 8) {		/* backspace */
				bs(vc);
				start = (ushort *)vc->vc_pos;
				start_x = vc->vc_x;
				continue;
			}
			if (c != 13)
				lf(vc);
			cr(vc);
			start = (ushort *)vc->vc_pos;
			start_x = vc->vc_x;
			if (c == 10 || c == 13)
				continue;
		}
		vc_uniscr_putc(vc, c);
		scr_writew((vc->vc_attr << 8) + c, (unsigned short *)vc->vc_pos);
		notify_write(vc, c);
		cnt++;
		if (vc->vc_x == vc->vc_cols - 1) {
			vc->vc_need_wrap = 1;
		} else {
			vc->vc_pos += 2;
			vc->vc_x++;
		}
	}
	if (cnt && con_is_visible(vc))
		vc->vc_sw->con_putcs(vc, start, cnt, vc->vc_y, start_x);
	set_cursor(vc);
	notify_update(vc);

quit:
	spin_unlock(&printing_lock);
}