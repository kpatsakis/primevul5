find_eol_char(char *s, size_t len)
{
#define CHUNK_SZ 128
	/* Lots of benchmarking found this approach to be faster in practice
	 * than doing two memchrs over the whole buffer, doin a memchr on each
	 * char of the buffer, or trying to emulate memchr by hand. */
	char *s_end, *cr, *lf;
	s_end = s+len;
	while (s < s_end) {
		size_t chunk = (s + CHUNK_SZ < s_end) ? CHUNK_SZ : (s_end - s);
		cr = memchr(s, '\r', chunk);
		lf = memchr(s, '\n', chunk);
		if (cr) {
			if (lf && lf < cr)
				return lf;
			return cr;
		} else if (lf) {
			return lf;
		}
		s += CHUNK_SZ;
	}

	return NULL;
#undef CHUNK_SZ
}