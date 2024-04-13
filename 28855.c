poppler_mapping_free (PopplerLinkMapping *mapping)
{
  poppler_action_free (mapping->action);
  g_free (mapping);
}
