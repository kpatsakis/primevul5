static void csi_K(struct vc_data *vc, int vpar)
{
	unsigned int count;
	unsigned short *start = (unsigned short *)vc->vc_pos;
	int offset;

	switch (vpar) {
		case 0:	/* erase from cursor to end of line */
			offset = 0;
			count = vc->vc_cols - vc->vc_x;
			break;
		case 1:	/* erase from start of line to cursor */
			offset = -vc->vc_x;
			count = vc->vc_x + 1;
			break;
		case 2: /* erase whole line */
			offset = -vc->vc_x;
			count = vc->vc_cols;
			break;
		default:
			return;
	}
	vc_uniscr_clear_line(vc, vc->vc_x + offset, count);
	scr_memsetw(start + offset, vc->vc_video_erase_char, 2 * count);
	vc->vc_need_wrap = 0;
	if (con_should_update(vc))
		do_update_region(vc, (unsigned long)(start + offset), count);
}