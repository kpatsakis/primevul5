_PUBLIC_ ssize_t push_codepoint_handle(struct smb_iconv_handle *ic,
				char *str, codepoint_t c)
{
	smb_iconv_t descriptor;
	uint8_t buf[4];
	size_t ilen, olen;
	const char *inbuf;

	if (c < 128) {
		*str = c;
		return 1;
	}

	descriptor = get_conv_handle(ic,
				     CH_UTF16, CH_UNIX);
	if (descriptor == (smb_iconv_t)-1) {
		return -1;
	}

	if (c < 0x10000) {
		ilen = 2;
		olen = 5;
		inbuf = (char *)buf;
		SSVAL(buf, 0, c);
		smb_iconv(descriptor, &inbuf, &ilen, &str, &olen);
		if (ilen != 0) {
			return -1;
		}
		return 5 - olen;
	}

	c -= 0x10000;

	buf[0] = (c>>10) & 0xFF;
	buf[1] = (c>>18) | 0xd8;
	buf[2] = c & 0xFF;
	buf[3] = ((c>>8) & 0x3) | 0xdc;

	ilen = 4;
	olen = 5;
	inbuf = (char *)buf;

	smb_iconv(descriptor, &inbuf, &ilen, &str, &olen);
	if (ilen != 0) {
		return -1;
	}
	return 5 - olen;
}
