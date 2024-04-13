mbfl_filt_conv_wchar_big5(int c, mbfl_convert_filter *filter)
{
	int k;
	int c1, s, c2;

	s = 0;
	if (c >= ucs_a1_big5_table_min && c < ucs_a1_big5_table_max) {
		s = ucs_a1_big5_table[c - ucs_a1_big5_table_min];
	} else if (c >= ucs_a2_big5_table_min && c < ucs_a2_big5_table_max) {
		s = ucs_a2_big5_table[c - ucs_a2_big5_table_min];
	} else if (c >= ucs_a3_big5_table_min && c < ucs_a3_big5_table_max) {
		s = ucs_a3_big5_table[c - ucs_a3_big5_table_min];
	} else if (c >= ucs_i_big5_table_min && c < ucs_i_big5_table_max) {
		s = ucs_i_big5_table[c - ucs_i_big5_table_min];
	} else if (c >= ucs_pua_big5_table_min && c < ucs_pua_big5_table_max) {
		s = ucs_pua_big5_table[c - ucs_pua_big5_table_min];
	} else if (c >= ucs_r1_big5_table_min && c < ucs_r1_big5_table_max) {
		s = ucs_r1_big5_table[c - ucs_r1_big5_table_min];
	} else if (c >= ucs_r2_big5_table_min && c < ucs_r2_big5_table_max) {
		s = ucs_r2_big5_table[c - ucs_r2_big5_table_min];
	}

	if (filter->to->no_encoding == mbfl_no_encoding_cp950) {
		if (c >= 0xe000 && c <= 0xf848) { /* PUA for CP950 */
			for (k = 0; k < sizeof(cp950_pua_tbl)/(sizeof(unsigned short)*4); k++) {
				if (c <= cp950_pua_tbl[k][1]) {
					break;
				}
			}
			c1 = c - cp950_pua_tbl[k][0];
			if ((cp950_pua_tbl[k][2] & 0xff) == 0x40) {
				c2 = cp950_pua_tbl[k][2] >> 8;
				s = ((c1 / 157) + c2) << 8; c1 %= 157;
				s |= c1 + (c1 >= 0x3f ? 0x62 : 0x40);
			} else {
				s = c1 + cp950_pua_tbl[k][2];
			}
		}

		if (c == 0x80) {
			s = 0x80;
		} else if (c == 0xf8f8) {
			s = 0xff;
		} else if (c == 0x256d) {
			s = 0xa27e;
		} else if (c == 0x256e) {
			s = 0xa2a1;
		} else if (c == 0x256f) {
			s = 0xa2a3;
		} else if (c == 0x2570) {
			s = 0xa2a2;
		}
	}

	if (s <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_BIG5) {
			s = c & MBFL_WCSPLANE_MASK;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s <= 0x80 || s == 0xff) {	/* latin */
			CK((*filter->output_function)(s, filter->data));
		} else {
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}