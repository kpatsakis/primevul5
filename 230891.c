static void setterm_command(struct vc_data *vc)
{
	switch (vc->vc_par[0]) {
	case 1:	/* set color for underline mode */
		if (vc->vc_can_do_color && vc->vc_par[1] < 16) {
			vc->vc_ulcolor = color_table[vc->vc_par[1]];
			if (vc->vc_underline)
				update_attr(vc);
		}
		break;
	case 2:	/* set color for half intensity mode */
		if (vc->vc_can_do_color && vc->vc_par[1] < 16) {
			vc->vc_halfcolor = color_table[vc->vc_par[1]];
			if (vc->vc_intensity == 0)
				update_attr(vc);
		}
		break;
	case 8:	/* store colors as defaults */
		vc->vc_def_color = vc->vc_attr;
		if (vc->vc_hi_font_mask == 0x100)
			vc->vc_def_color >>= 1;
		default_attr(vc);
		update_attr(vc);
		break;
	case 9:	/* set blanking interval */
		blankinterval = min(vc->vc_par[1], 60U) * 60;
		poke_blanked_console();
		break;
	case 10: /* set bell frequency in Hz */
		if (vc->vc_npar >= 1)
			vc->vc_bell_pitch = vc->vc_par[1];
		else
			vc->vc_bell_pitch = DEFAULT_BELL_PITCH;
		break;
	case 11: /* set bell duration in msec */
		if (vc->vc_npar >= 1)
			vc->vc_bell_duration = (vc->vc_par[1] < 2000) ?
				msecs_to_jiffies(vc->vc_par[1]) : 0;
		else
			vc->vc_bell_duration = DEFAULT_BELL_DURATION;
		break;
	case 12: /* bring specified console to the front */
		if (vc->vc_par[1] >= 1 && vc_cons_allocated(vc->vc_par[1] - 1))
			set_console(vc->vc_par[1] - 1);
		break;
	case 13: /* unblank the screen */
		poke_blanked_console();
		break;
	case 14: /* set vesa powerdown interval */
		vesa_off_interval = min(vc->vc_par[1], 60U) * 60 * HZ;
		break;
	case 15: /* activate the previous console */
		set_console(last_console);
		break;
	case 16: /* set cursor blink duration in msec */
		if (vc->vc_npar >= 1 && vc->vc_par[1] >= 50 &&
				vc->vc_par[1] <= USHRT_MAX)
			vc->vc_cur_blink_ms = vc->vc_par[1];
		else
			vc->vc_cur_blink_ms = DEFAULT_CURSOR_BLINK_MS;
		break;
	}
}