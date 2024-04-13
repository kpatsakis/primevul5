static void rgb_background(struct vc_data *vc, const struct rgb *c)
{
	/* For backgrounds, err on the dark side. */
	vc->vc_color = (vc->vc_color & 0x0f)
		| (c->r&0x80) >> 1 | (c->g&0x80) >> 2 | (c->b&0x80) >> 3;
}