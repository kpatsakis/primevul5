static void rgb_foreground(struct vc_data *vc, const struct rgb *c)
{
	u8 hue = 0, max = max3(c->r, c->g, c->b);

	if (c->r > max / 2)
		hue |= 4;
	if (c->g > max / 2)
		hue |= 2;
	if (c->b > max / 2)
		hue |= 1;

	if (hue == 7 && max <= 0x55) {
		hue = 0;
		vc->vc_intensity = 2;
	} else if (max > 0xaa)
		vc->vc_intensity = 2;
	else
		vc->vc_intensity = 1;

	vc->vc_color = (vc->vc_color & 0xf0) | hue;
}