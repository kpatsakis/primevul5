unsigned short *screen_pos(struct vc_data *vc, int w_offset, int viewed)
{
	return screenpos(vc, 2 * w_offset, viewed);
}