static void csi_J(struct vc_data *vc, int vpar)
{
	unsigned int count;
	unsigned short * start;

	switch (vpar) {
		case 0:	/* erase from cursor to end of display */
			vc_uniscr_clear_line(vc, vc->vc_x,
					     vc->vc_cols - vc->vc_x);
			vc_uniscr_clear_lines(vc, vc->vc_y + 1,
					      vc->vc_rows - vc->vc_y - 1);
			count = (vc->vc_scr_end - vc->vc_pos) >> 1;
			start = (unsigned short *)vc->vc_pos;
			break;
		case 1:	/* erase from start to cursor */
			vc_uniscr_clear_line(vc, 0, vc->vc_x + 1);
			vc_uniscr_clear_lines(vc, 0, vc->vc_y);
			count = ((vc->vc_pos - vc->vc_origin) >> 1) + 1;
			start = (unsigned short *)vc->vc_origin;
			break;
		case 3: /* include scrollback */
			flush_scrollback(vc);
			/* fallthrough */
		case 2: /* erase whole display */
			vc_uniscr_clear_lines(vc, 0, vc->vc_rows);
			count = vc->vc_cols * vc->vc_rows;
			start = (unsigned short *)vc->vc_origin;
			break;
		default:
			return;
	}
	scr_memsetw(start, vc->vc_video_erase_char, 2 * count);
	if (con_should_update(vc))
		do_update_region(vc, (unsigned long) start, count);
	vc->vc_need_wrap = 0;
}