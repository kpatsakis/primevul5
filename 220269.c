free_sconv_object(struct archive_string_conv *sc)
{
	free(sc->from_charset);
	free(sc->to_charset);
	archive_string_free(&sc->utftmp);
#if HAVE_ICONV
	if (sc->cd != (iconv_t)-1)
		iconv_close(sc->cd);
	if (sc->cd_w != (iconv_t)-1)
		iconv_close(sc->cd_w);
#endif
	free(sc);
}