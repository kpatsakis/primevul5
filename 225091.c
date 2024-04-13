static u8 vgacon_build_attr(struct vc_data *c, u8 color,
			    enum vc_intensity intensity,
			    bool blink, bool underline, bool reverse,
			    bool italic)
{
	u8 attr = color;

	if (vga_can_do_color) {
		if (italic)
			attr = (attr & 0xF0) | c->vc_itcolor;
		else if (underline)
			attr = (attr & 0xf0) | c->vc_ulcolor;
		else if (intensity == VCI_HALF_BRIGHT)
			attr = (attr & 0xf0) | c->vc_halfcolor;
	}
	if (reverse)
		attr =
		    ((attr) & 0x88) | ((((attr) >> 4) | ((attr) << 4)) &
				       0x77);
	if (blink)
		attr ^= 0x80;
	if (intensity == VCI_BOLD)
		attr ^= 0x08;
	if (!vga_can_do_color) {
		if (italic)
			attr = (attr & 0xF8) | 0x02;
		else if (underline)
			attr = (attr & 0xf8) | 0x01;
		else if (intensity == VCI_HALF_BRIGHT)
			attr = (attr & 0xf0) | 0x08;
	}
	return attr;
}