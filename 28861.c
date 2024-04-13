poppler_page_free_form_field_mapping (GList *list)
{
  if (list == NULL)
    return;

  g_list_foreach (list, (GFunc) poppler_form_field_mapping_free, NULL);
  g_list_free (list);
}
