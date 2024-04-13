static int close_iconv_handle(struct smb_iconv_handle *data)
{
	unsigned c1, c2;
	for (c1=0;c1<NUM_CHARSETS;c1++) {
		for (c2=0;c2<NUM_CHARSETS;c2++) {
			if (data->conv_handles[c1][c2] != NULL) {
				if (data->conv_handles[c1][c2] != (smb_iconv_t)-1) {
					smb_iconv_close(data->conv_handles[c1][c2]);
				}
				data->conv_handles[c1][c2] = NULL;
			}
		}
	}

	return 0;
}
