static int vgacon_adjust_height(struct vc_data *vc, unsigned fontheight)
{
	unsigned char ovr, vde, fsr;
	int rows, maxscan, i;

	rows = vc->vc_scan_lines / fontheight;	/* Number of video rows we end up with */
	maxscan = rows * fontheight - 1;	/* Scan lines to actually display-1 */

	/* Reprogram the CRTC for the new font size
	   Note: the attempt to read the overflow register will fail
	   on an EGA, but using 0xff for the previous value appears to
	   be OK for EGA text modes in the range 257-512 scan lines, so I
	   guess we don't need to worry about it.

	   The same applies for the spill bits in the font size and cursor
	   registers; they are write-only on EGA, but it appears that they
	   are all don't care bits on EGA, so I guess it doesn't matter. */

	raw_spin_lock_irq(&vga_lock);
	outb_p(0x07, vga_video_port_reg);	/* CRTC overflow register */
	ovr = inb_p(vga_video_port_val);
	outb_p(0x09, vga_video_port_reg);	/* Font size register */
	fsr = inb_p(vga_video_port_val);
	raw_spin_unlock_irq(&vga_lock);

	vde = maxscan & 0xff;	/* Vertical display end reg */
	ovr = (ovr & 0xbd) +	/* Overflow register */
	    ((maxscan & 0x100) >> 7) + ((maxscan & 0x200) >> 3);
	fsr = (fsr & 0xe0) + (fontheight - 1);	/*  Font size register */

	raw_spin_lock_irq(&vga_lock);
	outb_p(0x07, vga_video_port_reg);	/* CRTC overflow register */
	outb_p(ovr, vga_video_port_val);
	outb_p(0x09, vga_video_port_reg);	/* Font size */
	outb_p(fsr, vga_video_port_val);
	outb_p(0x12, vga_video_port_reg);	/* Vertical display limit */
	outb_p(vde, vga_video_port_val);
	raw_spin_unlock_irq(&vga_lock);
	vga_video_font_height = fontheight;

	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		struct vc_data *c = vc_cons[i].d;

		if (c && c->vc_sw == &vga_con) {
			if (con_is_visible(c)) {
			        /* void size to cause regs to be rewritten */
				cursor_size_lastfrom = 0;
				cursor_size_lastto = 0;
				c->vc_sw->con_cursor(c, CM_DRAW);
			}
			c->vc_font.height = fontheight;
			vc_resize(c, 0, rows);	/* Adjust console size */
		}
	}
	return 0;
}