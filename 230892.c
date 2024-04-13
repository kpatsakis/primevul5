void getconsxy(struct vc_data *vc, unsigned char *p)
{
	/* clamp values if they don't fit */
	p[0] = min(vc->vc_x, 0xFFu);
	p[1] = min(vc->vc_y, 0xFFu);
}