static void delete_char(struct vc_data *vc, unsigned int nr)
{
	unsigned short *p = (unsigned short *) vc->vc_pos;

	vc_uniscr_delete(vc, nr);
	scr_memcpyw(p, p + nr, (vc->vc_cols - vc->vc_x - nr) * 2);
	scr_memsetw(p + vc->vc_cols - vc->vc_x - nr, vc->vc_video_erase_char,
			nr * 2);
	vc->vc_need_wrap = 0;
	if (con_should_update(vc))
		do_update_region(vc, (unsigned long) p,
			vc->vc_cols - vc->vc_x);
}