static const char *default_charset(void)
{
# if defined HAVE_LIBCHARSET_H && defined HAVE_LOCALE_CHARSET
	return locale_charset();
# elif defined HAVE_LANGINFO_H && defined HAVE_NL_LANGINFO
	return nl_langinfo(CODESET);
# else
	return ""; /* Works with (at the very least) gnu iconv... */
# endif
}
