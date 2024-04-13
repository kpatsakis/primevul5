poppler_page_get_link_mapping (PopplerPage *page)
{
  GList *map_list = NULL;
  gint i;
  Links *links;
  Object obj;
  double width, height;
  
  g_return_val_if_fail (POPPLER_IS_PAGE (page), NULL);
  
  links = new Links (page->page->getAnnots (&obj),
		     page->document->doc->getCatalog ()->getBaseURI ());
  obj.free ();
  
  if (links == NULL)
    return NULL;
  
  poppler_page_get_size (page, &width, &height);
  
  for (i = 0; i < links->getNumLinks (); i++)
    {
      PopplerLinkMapping *mapping;
      PopplerRectangle rect;
      LinkAction *link_action;
      Link *link;
      
      link = links->getLink (i);
      link_action = link->getAction ();
      
      /* Create the mapping */
      mapping = g_new (PopplerLinkMapping, 1);
      mapping->action = _poppler_action_new (page->document, link_action, NULL);
      
      link->getRect (&rect.x1, &rect.y1, &rect.x2, &rect.y2);
      
      switch (page->page->getRotate ())
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
  
  delete links;
  
  return map_list;
}
