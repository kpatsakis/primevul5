static void restore_cur(struct vc_data *vc)
{
	gotoxy(vc, vc->vc_saved_x, vc->vc_saved_y);
	vc->vc_intensity	= vc->vc_s_intensity;
	vc->vc_italic		= vc->vc_s_italic;
	vc->vc_underline	= vc->vc_s_underline;
	vc->vc_blink		= vc->vc_s_blink;
	vc->vc_reverse		= vc->vc_s_reverse;
	vc->vc_charset		= vc->vc_s_charset;
	vc->vc_color		= vc->vc_s_color;
	vc->vc_G0_charset	= vc->vc_saved_G0;
	vc->vc_G1_charset	= vc->vc_saved_G1;
	vc->vc_translate	= set_translate(vc->vc_charset ? vc->vc_G1_charset : vc->vc_G0_charset, vc);
	update_attr(vc);
	vc->vc_need_wrap = 0;
}