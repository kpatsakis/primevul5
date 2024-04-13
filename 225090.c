static void vgacon_set_cursor_size(int xpos, int from, int to)
{
	unsigned long flags;
	int curs, cure;

	if ((from == cursor_size_lastfrom) && (to == cursor_size_lastto))
		return;
	cursor_size_lastfrom = from;
	cursor_size_lastto = to;

	raw_spin_lock_irqsave(&vga_lock, flags);
	if (vga_video_type >= VIDEO_TYPE_VGAC) {
		outb_p(VGA_CRTC_CURSOR_START, vga_video_port_reg);
		curs = inb_p(vga_video_port_val);
		outb_p(VGA_CRTC_CURSOR_END, vga_video_port_reg);
		cure = inb_p(vga_video_port_val);
	} else {
		curs = 0;
		cure = 0;
	}

	curs = (curs & 0xc0) | from;
	cure = (cure & 0xe0) | to;

	outb_p(VGA_CRTC_CURSOR_START, vga_video_port_reg);
	outb_p(curs, vga_video_port_val);
	outb_p(VGA_CRTC_CURSOR_END, vga_video_port_reg);
	outb_p(cure, vga_video_port_val);
	raw_spin_unlock_irqrestore(&vga_lock, flags);
}