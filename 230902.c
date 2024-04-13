static void reset_terminal(struct vc_data *vc, int do_clear)
{
	vc->vc_top		= 0;
	vc->vc_bottom		= vc->vc_rows;
	vc->vc_state		= ESnormal;
	vc->vc_priv		= EPecma;
	vc->vc_translate	= set_translate(LAT1_MAP, vc);
	vc->vc_G0_charset	= LAT1_MAP;
	vc->vc_G1_charset	= GRAF_MAP;
	vc->vc_charset		= 0;
	vc->vc_need_wrap	= 0;
	vc->vc_report_mouse	= 0;
	vc->vc_utf              = default_utf8;
	vc->vc_utf_count	= 0;

	vc->vc_disp_ctrl	= 0;
	vc->vc_toggle_meta	= 0;

	vc->vc_decscnm		= 0;
	vc->vc_decom		= 0;
	vc->vc_decawm		= 1;
	vc->vc_deccm		= global_cursor_default;
	vc->vc_decim		= 0;

	vt_reset_keyboard(vc->vc_num);

	vc->vc_cursor_type = cur_default;
	vc->vc_complement_mask = vc->vc_s_complement_mask;

	default_attr(vc);
	update_attr(vc);

	vc->vc_tab_stop[0]	=
	vc->vc_tab_stop[1]	=
	vc->vc_tab_stop[2]	=
	vc->vc_tab_stop[3]	=
	vc->vc_tab_stop[4]	=
	vc->vc_tab_stop[5]	=
	vc->vc_tab_stop[6]	=
	vc->vc_tab_stop[7]	= 0x01010101;

	vc->vc_bell_pitch = DEFAULT_BELL_PITCH;
	vc->vc_bell_duration = DEFAULT_BELL_DURATION;
	vc->vc_cur_blink_ms = DEFAULT_CURSOR_BLINK_MS;

	gotoxy(vc, 0, 0);
	save_cur(vc);
	if (do_clear)
	    csi_J(vc, 2);
}