static void do_con_trol(struct tty_struct *tty, struct vc_data *vc, int c)
{
	/*
	 *  Control characters can be used in the _middle_
	 *  of an escape sequence.
	 */
	if (vc->vc_state == ESosc && c>=8 && c<=13) /* ... except for OSC */
		return;
	switch (c) {
	case 0:
		return;
	case 7:
		if (vc->vc_state == ESosc)
			vc->vc_state = ESnormal;
		else if (vc->vc_bell_duration)
			kd_mksound(vc->vc_bell_pitch, vc->vc_bell_duration);
		return;
	case 8:
		bs(vc);
		return;
	case 9:
		vc->vc_pos -= (vc->vc_x << 1);
		while (vc->vc_x < vc->vc_cols - 1) {
			vc->vc_x++;
			if (vc->vc_tab_stop[7 & (vc->vc_x >> 5)] & (1 << (vc->vc_x & 31)))
				break;
		}
		vc->vc_pos += (vc->vc_x << 1);
		notify_write(vc, '\t');
		return;
	case 10: case 11: case 12:
		lf(vc);
		if (!is_kbd(vc, lnm))
			return;
		/* fall through */
	case 13:
		cr(vc);
		return;
	case 14:
		vc->vc_charset = 1;
		vc->vc_translate = set_translate(vc->vc_G1_charset, vc);
		vc->vc_disp_ctrl = 1;
		return;
	case 15:
		vc->vc_charset = 0;
		vc->vc_translate = set_translate(vc->vc_G0_charset, vc);
		vc->vc_disp_ctrl = 0;
		return;
	case 24: case 26:
		vc->vc_state = ESnormal;
		return;
	case 27:
		vc->vc_state = ESesc;
		return;
	case 127:
		del(vc);
		return;
	case 128+27:
		vc->vc_state = ESsquare;
		return;
	}
	switch(vc->vc_state) {
	case ESesc:
		vc->vc_state = ESnormal;
		switch (c) {
		case '[':
			vc->vc_state = ESsquare;
			return;
		case ']':
			vc->vc_state = ESnonstd;
			return;
		case '%':
			vc->vc_state = ESpercent;
			return;
		case 'E':
			cr(vc);
			lf(vc);
			return;
		case 'M':
			ri(vc);
			return;
		case 'D':
			lf(vc);
			return;
		case 'H':
			vc->vc_tab_stop[7 & (vc->vc_x >> 5)] |= (1 << (vc->vc_x & 31));
			return;
		case 'Z':
			respond_ID(tty);
			return;
		case '7':
			save_cur(vc);
			return;
		case '8':
			restore_cur(vc);
			return;
		case '(':
			vc->vc_state = ESsetG0;
			return;
		case ')':
			vc->vc_state = ESsetG1;
			return;
		case '#':
			vc->vc_state = EShash;
			return;
		case 'c':
			reset_terminal(vc, 1);
			return;
		case '>':  /* Numeric keypad */
			clr_kbd(vc, kbdapplic);
			return;
		case '=':  /* Appl. keypad */
			set_kbd(vc, kbdapplic);
			return;
		}
		return;
	case ESnonstd:
		if (c=='P') {   /* palette escape sequence */
			for (vc->vc_npar = 0; vc->vc_npar < NPAR; vc->vc_npar++)
				vc->vc_par[vc->vc_npar] = 0;
			vc->vc_npar = 0;
			vc->vc_state = ESpalette;
			return;
		} else if (c=='R') {   /* reset palette */
			reset_palette(vc);
			vc->vc_state = ESnormal;
		} else if (c>='0' && c<='9')
			vc->vc_state = ESosc;
		else
			vc->vc_state = ESnormal;
		return;
	case ESpalette:
		if (isxdigit(c)) {
			vc->vc_par[vc->vc_npar++] = hex_to_bin(c);
			if (vc->vc_npar == 7) {
				int i = vc->vc_par[0] * 3, j = 1;
				vc->vc_palette[i] = 16 * vc->vc_par[j++];
				vc->vc_palette[i++] += vc->vc_par[j++];
				vc->vc_palette[i] = 16 * vc->vc_par[j++];
				vc->vc_palette[i++] += vc->vc_par[j++];
				vc->vc_palette[i] = 16 * vc->vc_par[j++];
				vc->vc_palette[i] += vc->vc_par[j];
				set_palette(vc);
				vc->vc_state = ESnormal;
			}
		} else
			vc->vc_state = ESnormal;
		return;
	case ESsquare:
		for (vc->vc_npar = 0; vc->vc_npar < NPAR; vc->vc_npar++)
			vc->vc_par[vc->vc_npar] = 0;
		vc->vc_npar = 0;
		vc->vc_state = ESgetpars;
		if (c == '[') { /* Function key */
			vc->vc_state=ESfunckey;
			return;
		}
		switch (c) {
		case '?':
			vc->vc_priv = EPdec;
			return;
		case '>':
			vc->vc_priv = EPgt;
			return;
		case '=':
			vc->vc_priv = EPeq;
			return;
		case '<':
			vc->vc_priv = EPlt;
			return;
		}
		vc->vc_priv = EPecma;
		/* fall through */
	case ESgetpars:
		if (c == ';' && vc->vc_npar < NPAR - 1) {
			vc->vc_npar++;
			return;
		} else if (c>='0' && c<='9') {
			vc->vc_par[vc->vc_npar] *= 10;
			vc->vc_par[vc->vc_npar] += c - '0';
			return;
		}
		if (c >= 0x20 && c <= 0x3f) { /* 0x2x, 0x3a and 0x3c - 0x3f */
			vc->vc_state = EScsiignore;
			return;
		}
		vc->vc_state = ESnormal;
		switch(c) {
		case 'h':
			if (vc->vc_priv <= EPdec)
				set_mode(vc, 1);
			return;
		case 'l':
			if (vc->vc_priv <= EPdec)
				set_mode(vc, 0);
			return;
		case 'c':
			if (vc->vc_priv == EPdec) {
				if (vc->vc_par[0])
					vc->vc_cursor_type = vc->vc_par[0] | (vc->vc_par[1] << 8) | (vc->vc_par[2] << 16);
				else
					vc->vc_cursor_type = cur_default;
				return;
			}
			break;
		case 'm':
			if (vc->vc_priv == EPdec) {
				clear_selection();
				if (vc->vc_par[0])
					vc->vc_complement_mask = vc->vc_par[0] << 8 | vc->vc_par[1];
				else
					vc->vc_complement_mask = vc->vc_s_complement_mask;
				return;
			}
			break;
		case 'n':
			if (vc->vc_priv == EPecma) {
				if (vc->vc_par[0] == 5)
					status_report(tty);
				else if (vc->vc_par[0] == 6)
					cursor_report(vc, tty);
			}
			return;
		}
		if (vc->vc_priv != EPecma) {
			vc->vc_priv = EPecma;
			return;
		}
		switch(c) {
		case 'G': case '`':
			if (vc->vc_par[0])
				vc->vc_par[0]--;
			gotoxy(vc, vc->vc_par[0], vc->vc_y);
			return;
		case 'A':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			gotoxy(vc, vc->vc_x, vc->vc_y - vc->vc_par[0]);
			return;
		case 'B': case 'e':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			gotoxy(vc, vc->vc_x, vc->vc_y + vc->vc_par[0]);
			return;
		case 'C': case 'a':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			gotoxy(vc, vc->vc_x + vc->vc_par[0], vc->vc_y);
			return;
		case 'D':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			gotoxy(vc, vc->vc_x - vc->vc_par[0], vc->vc_y);
			return;
		case 'E':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			gotoxy(vc, 0, vc->vc_y + vc->vc_par[0]);
			return;
		case 'F':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			gotoxy(vc, 0, vc->vc_y - vc->vc_par[0]);
			return;
		case 'd':
			if (vc->vc_par[0])
				vc->vc_par[0]--;
			gotoxay(vc, vc->vc_x ,vc->vc_par[0]);
			return;
		case 'H': case 'f':
			if (vc->vc_par[0])
				vc->vc_par[0]--;
			if (vc->vc_par[1])
				vc->vc_par[1]--;
			gotoxay(vc, vc->vc_par[1], vc->vc_par[0]);
			return;
		case 'J':
			csi_J(vc, vc->vc_par[0]);
			return;
		case 'K':
			csi_K(vc, vc->vc_par[0]);
			return;
		case 'L':
			csi_L(vc, vc->vc_par[0]);
			return;
		case 'M':
			csi_M(vc, vc->vc_par[0]);
			return;
		case 'P':
			csi_P(vc, vc->vc_par[0]);
			return;
		case 'c':
			if (!vc->vc_par[0])
				respond_ID(tty);
			return;
		case 'g':
			if (!vc->vc_par[0])
				vc->vc_tab_stop[7 & (vc->vc_x >> 5)] &= ~(1 << (vc->vc_x & 31));
			else if (vc->vc_par[0] == 3) {
				vc->vc_tab_stop[0] =
					vc->vc_tab_stop[1] =
					vc->vc_tab_stop[2] =
					vc->vc_tab_stop[3] =
					vc->vc_tab_stop[4] =
					vc->vc_tab_stop[5] =
					vc->vc_tab_stop[6] =
					vc->vc_tab_stop[7] = 0;
			}
			return;
		case 'm':
			csi_m(vc);
			return;
		case 'q': /* DECLL - but only 3 leds */
			/* map 0,1,2,3 to 0,1,2,4 */
			if (vc->vc_par[0] < 4)
				vt_set_led_state(vc->vc_num,
					    (vc->vc_par[0] < 3) ? vc->vc_par[0] : 4);
			return;
		case 'r':
			if (!vc->vc_par[0])
				vc->vc_par[0]++;
			if (!vc->vc_par[1])
				vc->vc_par[1] = vc->vc_rows;
			/* Minimum allowed region is 2 lines */
			if (vc->vc_par[0] < vc->vc_par[1] &&
			    vc->vc_par[1] <= vc->vc_rows) {
				vc->vc_top = vc->vc_par[0] - 1;
				vc->vc_bottom = vc->vc_par[1];
				gotoxay(vc, 0, 0);
			}
			return;
		case 's':
			save_cur(vc);
			return;
		case 'u':
			restore_cur(vc);
			return;
		case 'X':
			csi_X(vc, vc->vc_par[0]);
			return;
		case '@':
			csi_at(vc, vc->vc_par[0]);
			return;
		case ']': /* setterm functions */
			setterm_command(vc);
			return;
		}
		return;
	case EScsiignore:
		if (c >= 20 && c <= 0x3f)
			return;
		vc->vc_state = ESnormal;
		return;
	case ESpercent:
		vc->vc_state = ESnormal;
		switch (c) {
		case '@':  /* defined in ISO 2022 */
			vc->vc_utf = 0;
			return;
		case 'G':  /* prelim official escape code */
		case '8':  /* retained for compatibility */
			vc->vc_utf = 1;
			return;
		}
		return;
	case ESfunckey:
		vc->vc_state = ESnormal;
		return;
	case EShash:
		vc->vc_state = ESnormal;
		if (c == '8') {
			/* DEC screen alignment test. kludge :-) */
			vc->vc_video_erase_char =
				(vc->vc_video_erase_char & 0xff00) | 'E';
			csi_J(vc, 2);
			vc->vc_video_erase_char =
				(vc->vc_video_erase_char & 0xff00) | ' ';
			do_update_region(vc, vc->vc_origin, vc->vc_screenbuf_size / 2);
		}
		return;
	case ESsetG0:
		if (c == '0')
			vc->vc_G0_charset = GRAF_MAP;
		else if (c == 'B')
			vc->vc_G0_charset = LAT1_MAP;
		else if (c == 'U')
			vc->vc_G0_charset = IBMPC_MAP;
		else if (c == 'K')
			vc->vc_G0_charset = USER_MAP;
		if (vc->vc_charset == 0)
			vc->vc_translate = set_translate(vc->vc_G0_charset, vc);
		vc->vc_state = ESnormal;
		return;
	case ESsetG1:
		if (c == '0')
			vc->vc_G1_charset = GRAF_MAP;
		else if (c == 'B')
			vc->vc_G1_charset = LAT1_MAP;
		else if (c == 'U')
			vc->vc_G1_charset = IBMPC_MAP;
		else if (c == 'K')
			vc->vc_G1_charset = USER_MAP;
		if (vc->vc_charset == 1)
			vc->vc_translate = set_translate(vc->vc_G1_charset, vc);
		vc->vc_state = ESnormal;
		return;
	case ESosc:
		return;
	default:
		vc->vc_state = ESnormal;
	}
}