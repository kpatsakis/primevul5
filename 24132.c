rpa_parse_token1(const void *data, size_t data_size, const char **error)
{
	const unsigned char *end = ((const unsigned char *) data) + data_size;
	const unsigned char *p;
	unsigned int version_lo, version_hi;

	p = rpa_check_message(data, end, error);
	if (p == NULL)
		return FALSE;

	if (p + 6 > end) {
		*error = "message too short";
		return FALSE;
	}

	version_lo = p[0] + (p[1] << 8);
	version_hi = p[2] + (p[3] << 8);

	if ((version_lo > 3) || (version_hi < 3)) {
		*error = "protocol version mismatch";
		return FALSE;
	}
	p += 4;

	if ((p[0] != 0) || (p[1] != 1)) {
		*error = "invalid message flags";
		return FALSE;
	}
	p += 2;

	if (p != end) {
		*error = "unneeded data found";
		return FALSE;
	}

	return TRUE;
}