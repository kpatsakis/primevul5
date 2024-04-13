get_cmdline_bad_pass_thresh(char *argv[], uint8_t *buf)
{
	uint16_t reset, lockout;

	if (str2uchar(argv[0], &buf[1])) {
		return -1;
	}

	if (str2uchar(argv[1], &buf[0]) || buf[0] > 1) {
		return -1;
	}

	if (str2ushort(argv[2], &reset)) {
		return -1;
	}

	if (str2ushort(argv[3], &lockout)) {
		return -1;
	}

	/* store parsed data */
	buf[2] = reset & 0xFF;
	buf[3] = reset >> 8;
	buf[4] = lockout & 0xFF;
	buf[5] = lockout >> 8;
	return 0;
}