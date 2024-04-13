poppler_page_free_annot_mapping (GList *list)
{
  if (!list)
    return;

  g_list_foreach (list, (GFunc)poppler_annot_mapping_free, NULL);
  g_list_free (list);
}
