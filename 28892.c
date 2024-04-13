poppler_page_selection_region_free (GList *region)
{
  if (!region)
    return;

  g_list_foreach (region, (GFunc)poppler_rectangle_free, NULL);
  g_list_free (region);
}
