static inline unsigned short *screenpos(struct vc_data *vc, int offset, int viewed)
{
	unsigned short *p;
	
	if (!viewed)
		p = (unsigned short *)(vc->vc_origin + offset);
	else if (!vc->vc_sw->con_screen_pos)
		p = (unsigned short *)(vc->vc_visible_origin + offset);
	else
		p = vc->vc_sw->con_screen_pos(vc, offset);
	return p;
}