invalid_mbs(const void *_p, size_t n, struct archive_string_conv *sc)
{
	const char *p = (const char *)_p;
	size_t r;

#if HAVE_MBRTOWC
	mbstate_t shift_state;

	memset(&shift_state, 0, sizeof(shift_state));
#else
	/* Clear the shift state before starting. */
	mbtowc(NULL, NULL, 0);
#endif
	while (n) {
		wchar_t wc;

#if HAVE_MBRTOWC
		r = mbrtowc(&wc, p, n, &shift_state);
#else
		r = mbtowc(&wc, p, n);
#endif
		if (r == (size_t)-1 || r == (size_t)-2)
			return (-1);/* Invalid. */
		if (r == 0)
			break;
		p += r;
		n -= r;
	}
	(void)sc; /* UNUSED */
	return (0); /* All Okey. */
}