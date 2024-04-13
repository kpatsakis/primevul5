static void set_origin(struct vc_data *vc)
{
	WARN_CONSOLE_UNLOCKED();

	if (!con_is_visible(vc) ||
	    !vc->vc_sw->con_set_origin ||
	    !vc->vc_sw->con_set_origin(vc))
		vc->vc_origin = (unsigned long)vc->vc_screenbuf;
	vc->vc_visible_origin = vc->vc_origin;
	vc->vc_scr_end = vc->vc_origin + vc->vc_screenbuf_size;
	vc->vc_pos = vc->vc_origin + vc->vc_size_row * vc->vc_y + 2 * vc->vc_x;
}