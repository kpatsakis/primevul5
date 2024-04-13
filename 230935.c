static void hide_softcursor(struct vc_data *vc)
{
	if (softcursor_original != -1) {
		scr_writew(softcursor_original, (u16 *)vc->vc_pos);
		if (con_should_update(vc))
			vc->vc_sw->con_putc(vc, softcursor_original,
					vc->vc_y, vc->vc_x);
		softcursor_original = -1;
	}
}