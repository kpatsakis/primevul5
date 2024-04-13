default_iconv_charset(const char *charset) {
	if (charset != NULL && charset[0] != '\0')
		return charset;
#if HAVE_LOCALE_CHARSET && !defined(__APPLE__)
	/* locale_charset() is broken on Mac OS */
	return locale_charset();
#elif HAVE_NL_LANGINFO
	return nl_langinfo(CODESET);
#else
	return "";
#endif
}