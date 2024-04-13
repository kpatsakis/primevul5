find_sconv_object(struct archive *a, const char *fc, const char *tc)
{
	struct archive_string_conv *sc; 

	if (a == NULL)
		return (NULL);

	for (sc = a->sconv; sc != NULL; sc = sc->next) {
		if (strcmp(sc->from_charset, fc) == 0 &&
		    strcmp(sc->to_charset, tc) == 0)
			break;
	}
	return (sc);
}