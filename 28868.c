poppler_page_get_form_field_mapping (PopplerPage *page)
{
  GList *map_list = NULL;
  FormPageWidgets *forms;
  gint i;
  
  g_return_val_if_fail (POPPLER_IS_PAGE (page), NULL);

  forms = page->page->getPageWidgets ();
  if (forms == NULL)
    return NULL;
  
  for (i = 0; i < forms->getNumWidgets (); i++) {
    PopplerFormFieldMapping *mapping;
    FormWidget *field;

    mapping = poppler_form_field_mapping_new ();
    
    field = forms->getWidget (i);

    mapping->field = _poppler_form_field_new (page->document, field);
    field->getRect (&(mapping->area.x1), &(mapping->area.y1),
		    &(mapping->area.x2), &(mapping->area.y2));

    mapping->area.x1 -= page->page->getCropBox()->x1;
    mapping->area.x2 -= page->page->getCropBox()->x1;
    mapping->area.y1 -= page->page->getCropBox()->y1;
    mapping->area.y2 -= page->page->getCropBox()->y1;
    
    map_list = g_list_prepend (map_list, mapping);
  }
  
  return map_list;
}
