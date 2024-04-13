void smb_init_locale(void)
{
	/* This is a useful global hook where we can ensure that the
	 * locale is set from the environment.  This is needed so that
	 * we can use LOCALE as a codepage */
#ifdef HAVE_SETLOCALE
	setlocale(LC_ALL, "");
#endif
}
