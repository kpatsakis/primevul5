add_sconv_object(struct archive *a, struct archive_string_conv *sc)
{
	struct archive_string_conv **psc; 

	/* Add a new sconv to sconv list. */
	psc = &(a->sconv);
	while (*psc != NULL)
		psc = &((*psc)->next);
	*psc = sc;
}