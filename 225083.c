static void vgacon_invert_region(struct vc_data *c, u16 * p, int count)
{
	const bool col = vga_can_do_color;

	while (count--) {
		u16 a = scr_readw(p);
		if (col)
			a = ((a) & 0x88ff) | (((a) & 0x7000) >> 4) |
			    (((a) & 0x0700) << 4);
		else
			a ^= ((a & 0x0700) == 0x0100) ? 0x7000 : 0x7700;
		scr_writew(a, p++);
	}
}