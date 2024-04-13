poppler_link_mapping_free (PopplerLinkMapping *mapping)
{
  if (mapping->action)
    poppler_action_free (mapping->action);

  g_free (mapping);
}
