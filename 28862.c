poppler_page_free_image_mapping (GList *list)
{
  if (list == NULL)
    return;

  g_list_foreach (list, (GFunc)g_free, NULL);
  g_list_free (list);
}
