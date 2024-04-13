poppler_link_mapping_copy (PopplerLinkMapping *mapping)
{
  PopplerLinkMapping *new_mapping;

  new_mapping = poppler_link_mapping_new ();
	
  *new_mapping = *mapping;
  if (new_mapping->action)
    new_mapping->action = poppler_action_copy (new_mapping->action);

  return new_mapping;
}
