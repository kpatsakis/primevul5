static void lf(struct vc_data *vc)
{
    	/* don't scroll if above bottom of scrolling region, or
	 * if below scrolling region
	 */
    	if (vc->vc_y + 1 == vc->vc_bottom)
		con_scroll(vc, vc->vc_top, vc->vc_bottom, SM_UP, 1);
	else if (vc->vc_y < vc->vc_rows - 1) {
	    	vc->vc_y++;
		vc->vc_pos += vc->vc_size_row;
	}
	vc->vc_need_wrap = 0;
	notify_write(vc, '\n');
}