static void csi_m(struct vc_data *vc)
{
	int i;

	for (i = 0; i <= vc->vc_npar; i++)
		switch (vc->vc_par[i]) {
		case 0:	/* all attributes off */
			default_attr(vc);
			break;
		case 1:
			vc->vc_intensity = 2;
			break;
		case 2:
			vc->vc_intensity = 0;
			break;
		case 3:
			vc->vc_italic = 1;
			break;
		case 21:
			/*
			 * No console drivers support double underline, so
			 * convert it to a single underline.
			 */
		case 4:
			vc->vc_underline = 1;
			break;
		case 5:
			vc->vc_blink = 1;
			break;
		case 7:
			vc->vc_reverse = 1;
			break;
		case 10: /* ANSI X3.64-1979 (SCO-ish?)
			  * Select primary font, don't display control chars if
			  * defined, don't set bit 8 on output.
			  */
			vc->vc_translate = set_translate(vc->vc_charset == 0
					? vc->vc_G0_charset
					: vc->vc_G1_charset, vc);
			vc->vc_disp_ctrl = 0;
			vc->vc_toggle_meta = 0;
			break;
		case 11: /* ANSI X3.64-1979 (SCO-ish?)
			  * Select first alternate font, lets chars < 32 be
			  * displayed as ROM chars.
			  */
			vc->vc_translate = set_translate(IBMPC_MAP, vc);
			vc->vc_disp_ctrl = 1;
			vc->vc_toggle_meta = 0;
			break;
		case 12: /* ANSI X3.64-1979 (SCO-ish?)
			  * Select second alternate font, toggle high bit
			  * before displaying as ROM char.
			  */
			vc->vc_translate = set_translate(IBMPC_MAP, vc);
			vc->vc_disp_ctrl = 1;
			vc->vc_toggle_meta = 1;
			break;
		case 22:
			vc->vc_intensity = 1;
			break;
		case 23:
			vc->vc_italic = 0;
			break;
		case 24:
			vc->vc_underline = 0;
			break;
		case 25:
			vc->vc_blink = 0;
			break;
		case 27:
			vc->vc_reverse = 0;
			break;
		case 38:
			i = vc_t416_color(vc, i, rgb_foreground);
			break;
		case 48:
			i = vc_t416_color(vc, i, rgb_background);
			break;
		case 39:
			vc->vc_color = (vc->vc_def_color & 0x0f) |
				(vc->vc_color & 0xf0);
			break;
		case 49:
			vc->vc_color = (vc->vc_def_color & 0xf0) |
				(vc->vc_color & 0x0f);
			break;
		default:
			if (vc->vc_par[i] >= 90 && vc->vc_par[i] <= 107) {
				if (vc->vc_par[i] < 100)
					vc->vc_intensity = 2;
				vc->vc_par[i] -= 60;
			}
			if (vc->vc_par[i] >= 30 && vc->vc_par[i] <= 37)
				vc->vc_color = color_table[vc->vc_par[i] - 30]
					| (vc->vc_color & 0xf0);
			else if (vc->vc_par[i] >= 40 && vc->vc_par[i] <= 47)
				vc->vc_color = (color_table[vc->vc_par[i] - 40] << 4)
					| (vc->vc_color & 0x0f);
			break;
		}
	update_attr(vc);
}