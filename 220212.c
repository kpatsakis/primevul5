archive_string_conversion_free(struct archive *a)
{
	struct archive_string_conv *sc; 
	struct archive_string_conv *sc_next; 

	for (sc = a->sconv; sc != NULL; sc = sc_next) {
		sc_next = sc->next;
		free_sconv_object(sc);
	}
	a->sconv = NULL;
	free(a->current_code);
	a->current_code = NULL;
}