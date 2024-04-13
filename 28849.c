poppler_image_mapping_copy (PopplerImageMapping *mapping)
{
  PopplerImageMapping *new_mapping;

  new_mapping = poppler_image_mapping_new ();

  *new_mapping = *mapping;

  return new_mapping;
}
