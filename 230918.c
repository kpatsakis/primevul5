static void set_cursor(struct vc_data *vc)
{
	if (!con_is_fg(vc) || console_blanked || vc->vc_mode == KD_GRAPHICS)
		return;
	if (vc->vc_deccm) {
		if (vc_is_sel(vc))
			clear_selection();
		add_softcursor(vc);
		if ((vc->vc_cursor_type & 0x0f) != 1)
			vc->vc_sw->con_cursor(vc, CM_DRAW);
	} else
		hide_cursor(vc);
}