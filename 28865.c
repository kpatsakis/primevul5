poppler_page_get_annot_mapping (PopplerPage *page)
{
  GList *map_list = NULL;
  double width, height;
  gint i;

  g_return_val_if_fail (POPPLER_IS_PAGE (page), NULL);

  if (!page->annots)
    page->annots = page->page->getAnnots (page->document->doc->getCatalog ());
  
  if (!page->annots)
    return NULL;

  poppler_page_get_size (page, &width, &height);

  for (i = 0; i < page->annots->getNumAnnots (); i++) {
    PopplerAnnotMapping *mapping;
    PopplerRectangle rect;
    Annot *annot;
    PDFRectangle *annot_rect;
    gint rotation = 0;

    annot = page->annots->getAnnot (i);

    /* Create the mapping */
    mapping = poppler_annot_mapping_new ();

    switch (annot->getType ())
      {
      case Annot::typeText:
        mapping->annot = _poppler_annot_text_new (annot);
	break;
      case Annot::typeFreeText:
        mapping->annot = _poppler_annot_free_text_new (annot);
	break;
      default:
        mapping->annot = _poppler_annot_new (annot);
	break;
      }

    annot_rect = annot->getRect ();
    rect.x1 = annot_rect->x1;
    rect.y1 = annot_rect->y1;
    rect.x2 = annot_rect->x2;
    rect.y2 = annot_rect->y2;

    if (! (annot->getFlags () & Annot::flagNoRotate))
      rotation = page->page->getRotate ();

    switch (rotation)
      {
      case 90:
        mapping->area.x1 = rect.y1;
        mapping->area.y1 = height - rect.x2;
        mapping->area.x2 = mapping->area.x1 + (rect.y2 - rect.y1);
        mapping->area.y2 = mapping->area.y1 + (rect.x2 - rect.x1);
        break;
      case 180:
        mapping->area.x1 = width - rect.x2;
        mapping->area.y1 = height - rect.y2;
        mapping->area.x2 = mapping->area.x1 + (rect.x2 - rect.x1);
        mapping->area.y2 = mapping->area.y1 + (rect.y2 - rect.y1);
        break;
      case 270:
        mapping->area.x1 = width - rect.y2;
        mapping->area.y1 = rect.x1;
        mapping->area.x2 = mapping->area.x1 + (rect.y2 - rect.y1);
        mapping->area.y2 = mapping->area.y1 + (rect.x2 - rect.x1);
        break;
      default:
        mapping->area.x1 = rect.x1;
        mapping->area.y1 = rect.y1;
        mapping->area.x2 = rect.x2;
        mapping->area.y2 = rect.y2;
      }

    mapping->area.x1 -= page->page->getCropBox()->x1;
    mapping->area.x2 -= page->page->getCropBox()->x1;
    mapping->area.y1 -= page->page->getCropBox()->y1;
    mapping->area.y2 -= page->page->getCropBox()->y1;

    map_list = g_list_prepend (map_list, mapping);
  }

  return g_list_reverse (map_list);
}
