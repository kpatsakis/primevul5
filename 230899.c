static void default_attr(struct vc_data *vc)
{
	vc->vc_intensity = 1;
	vc->vc_italic = 0;
	vc->vc_underline = 0;
	vc->vc_reverse = 0;
	vc->vc_blink = 0;
	vc->vc_color = vc->vc_def_color;
}