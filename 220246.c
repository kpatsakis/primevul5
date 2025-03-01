archive_string_conversion_set_opt(struct archive_string_conv *sc, int opt)
{
	switch (opt) {
	/*
	 * A filename in UTF-8 was made with libarchive 2.x in a wrong
	 * assumption that wchar_t was Unicode.
	 * This option enables simulating the assumption in order to read
	 * that filename correctly.
	 */
	case SCONV_SET_OPT_UTF8_LIBARCHIVE2X:
#if (defined(_WIN32) && !defined(__CYGWIN__)) \
	 || defined(__STDC_ISO_10646__) || defined(__APPLE__)
		/*
		 * Nothing to do for it since wchar_t on these platforms
		 * is really Unicode.
		 */
		(void)sc; /* UNUSED */
#else
		if ((sc->flag & SCONV_UTF8_LIBARCHIVE_2) == 0) {
			sc->flag |= SCONV_UTF8_LIBARCHIVE_2;
			/* Set up string converters. */
			setup_converter(sc);
		}
#endif
		break;
	case SCONV_SET_OPT_NORMALIZATION_C:
		if ((sc->flag & SCONV_NORMALIZATION_C) == 0) {
			sc->flag |= SCONV_NORMALIZATION_C;
			sc->flag &= ~SCONV_NORMALIZATION_D;
			/* Set up string converters. */
			setup_converter(sc);
		}
		break;
	case SCONV_SET_OPT_NORMALIZATION_D:
#if defined(HAVE_ICONV)
		/*
		 * If iconv will take the string, do not change the
		 * setting of the normalization.
		 */
		if (!(sc->flag & SCONV_WIN_CP) &&
		     (sc->flag & (SCONV_FROM_UTF16 | SCONV_FROM_UTF8)) &&
		    !(sc->flag & (SCONV_TO_UTF16 | SCONV_TO_UTF8)))
			break;
#endif
		if ((sc->flag & SCONV_NORMALIZATION_D) == 0) {
			sc->flag |= SCONV_NORMALIZATION_D;
			sc->flag &= ~SCONV_NORMALIZATION_C;
			/* Set up string converters. */
			setup_converter(sc);
		}
		break;
	default:
		break;
	}
}