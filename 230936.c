void complement_pos(struct vc_data *vc, int offset)
{
	static int old_offset = -1;
	static unsigned short old;
	static unsigned short oldx, oldy;

	WARN_CONSOLE_UNLOCKED();

	if (old_offset != -1 && old_offset >= 0 &&
	    old_offset < vc->vc_screenbuf_size) {
		scr_writew(old, screenpos(vc, old_offset, 1));
		if (con_should_update(vc))
			vc->vc_sw->con_putc(vc, old, oldy, oldx);
		notify_update(vc);
	}

	old_offset = offset;

	if (offset != -1 && offset >= 0 &&
	    offset < vc->vc_screenbuf_size) {
		unsigned short new;
		unsigned short *p;
		p = screenpos(vc, offset, 1);
		old = scr_readw(p);
		new = old ^ vc->vc_complement_mask;
		scr_writew(new, p);
		if (con_should_update(vc)) {
			oldx = (offset >> 1) % vc->vc_cols;
			oldy = (offset >> 1) / vc->vc_cols;
			vc->vc_sw->con_putc(vc, new, oldy, oldx);
		}
		notify_update(vc);
	}
}