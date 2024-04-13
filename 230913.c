static void ri(struct vc_data *vc)
{
    	/* don't scroll if below top of scrolling region, or
	 * if above scrolling region
	 */
	if (vc->vc_y == vc->vc_top)
		con_scroll(vc, vc->vc_top, vc->vc_bottom, SM_DOWN, 1);
	else if (vc->vc_y > 0) {
		vc->vc_y--;
		vc->vc_pos -= vc->vc_size_row;
	}
	vc->vc_need_wrap = 0;
}