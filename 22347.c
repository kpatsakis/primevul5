static int mbfl_filt_ident_big5(int c, mbfl_identify_filter *filter)
{
	int c1;
	if (filter->encoding->no_encoding == mbfl_no_encoding_cp950) {
		c1 = 0x80;
	} else {
		c1 = 0xa0;
	}

	if (filter->status) {		/* kanji second char */
		if (c < 0x40 || (c > 0x7e && c < 0xa1) ||c > 0xfe) {	/* bad */
		    filter->flag = 1;
		}
		filter->status = 0;
	} else if (c >= 0 && c < 0x80) {	/* latin  ok */
		;
	} else if (c > c1 && c < 0xff) {	/* DBCS lead byte */
		filter->status = 1;
	} else {							/* bad */
		filter->flag = 1;
	}

	return c;
}