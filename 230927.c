static void csi_X(struct vc_data *vc, int vpar) /* erase the following vpar positions */
{					  /* not vt100? */
	int count;

	if (!vpar)
		vpar++;
	count = (vpar > vc->vc_cols - vc->vc_x) ? (vc->vc_cols - vc->vc_x) : vpar;

	vc_uniscr_clear_line(vc, vc->vc_x, count);
	scr_memsetw((unsigned short *)vc->vc_pos, vc->vc_video_erase_char, 2 * count);
	if (con_should_update(vc))
		vc->vc_sw->con_clear(vc, vc->vc_y, vc->vc_x, 1, count);
	vc->vc_need_wrap = 0;
}