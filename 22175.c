comics_document_get_page_size (EvDocument *document,
			       EvPage     *page,
			       double     *width,
			       double     *height)
{
	GdkPixbufLoader *loader;
	char **argv;
	guchar buf[1024];
	gboolean success, got_size = FALSE;
	gint outpipe = -1;
	GPid child_pid;
	gssize bytes;
	GdkPixbuf *pixbuf;
	gchar *filename;
	ComicsDocument *comics_document = COMICS_DOCUMENT (document);
	
	if (!comics_document->decompress_tmp) {
		argv = extract_argv (document, page->index);
		success = g_spawn_async_with_pipes (NULL, argv, NULL,
						    G_SPAWN_SEARCH_PATH | 
						    G_SPAWN_STDERR_TO_DEV_NULL,
						    NULL, NULL,
						    &child_pid,
						    NULL, &outpipe, NULL, NULL);
		g_strfreev (argv);
		g_return_if_fail (success == TRUE);

		loader = gdk_pixbuf_loader_new ();
		g_signal_connect (loader, "area-prepared",
				  G_CALLBACK (get_page_size_area_prepared_cb),
				  &got_size);

		while (outpipe >= 0) {
			bytes = read (outpipe, buf, 1024);
		
			if (bytes > 0)
			gdk_pixbuf_loader_write (loader, buf, bytes, NULL);
			if (bytes <= 0 || got_size) {
				close (outpipe);
				outpipe = -1;
				gdk_pixbuf_loader_close (loader, NULL);
			}
		}
		pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
		if (pixbuf) {
			if (width)
				*width = gdk_pixbuf_get_width (pixbuf);
			if (height)
				*height = gdk_pixbuf_get_height (pixbuf);
		}
		g_spawn_close_pid (child_pid);
		g_object_unref (loader);
	} else {
		filename = g_build_filename (comics_document->dir,
                                             (char *) comics_document->page_names->pdata[page->index],
					     NULL);
		pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
		if (pixbuf) {
			if (width)
				*width = gdk_pixbuf_get_width (pixbuf);
			if (height)
				*height = gdk_pixbuf_get_height (pixbuf);
			g_object_unref (pixbuf);
		}
		g_free (filename);
	}
}