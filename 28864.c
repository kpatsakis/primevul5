poppler_page_free_link_mapping (GList *list)
{
  if (list == NULL)
    return;

  g_list_foreach (list, (GFunc) (poppler_mapping_free), NULL);
  g_list_free (list);
}
