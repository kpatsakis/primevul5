poppler_page_finalize (GObject *object)
{
  PopplerPage *page = POPPLER_PAGE (object);

  g_object_unref (page->document);
  page->document = NULL;

  if (page->annots != NULL)
    delete page->annots;
#if defined (HAVE_CAIRO)
  if (page->text != NULL) 
    page->text->decRefCnt();
#else
  if (page->gfx != NULL)
    delete page->gfx;  
  if (page->text_dev != NULL)
    delete page->text_dev;
#endif
  /* page->page is owned by the document */
}
