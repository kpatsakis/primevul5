comics_document_finalize (GObject *object)
{
	ComicsDocument *comics_document = COMICS_DOCUMENT (object);
	
	if (comics_document->decompress_tmp) {
		if (comics_remove_dir (comics_document->dir) == -1)
			g_warning (_("There was an error deleting “%s”."),
				   comics_document->dir);
		g_free (comics_document->dir);
	}
	
	if (comics_document->page_names) {
                g_ptr_array_foreach (comics_document->page_names, (GFunc) g_free, NULL);
                g_ptr_array_free (comics_document->page_names, TRUE);
	}

	g_free (comics_document->archive);
	g_free (comics_document->selected_command);
	g_free (comics_document->alternative_command);
	g_free (comics_document->extract_command);
	g_free (comics_document->list_command);

	G_OBJECT_CLASS (comics_document_parent_class)->finalize (object);
}