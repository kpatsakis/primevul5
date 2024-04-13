comics_document_get_n_pages (EvDocument *document)
{
	ComicsDocument *comics_document = COMICS_DOCUMENT (document);

        if (comics_document->page_names == NULL)
                return 0;

	return comics_document->page_names->len;
}