poppler_page_get_property (GObject *object,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *pspec)
{
  PopplerPage *page = POPPLER_PAGE (object);
  GooString label;

  switch (prop_id)
    {
    case PROP_LABEL:
      page->document->doc->getCatalog ()->indexToLabel (page->index, &label);
      g_value_take_string (value, _poppler_goo_string_to_utf8(&label));
      break;
    }
}
