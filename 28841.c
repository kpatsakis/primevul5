poppler_annot_mapping_copy (PopplerAnnotMapping *mapping)
{
  PopplerAnnotMapping *new_mapping;

  new_mapping = poppler_annot_mapping_new ();

  *new_mapping = *mapping;
  if (mapping->annot)
    new_mapping->annot = (PopplerAnnot *) g_object_ref (mapping->annot);

  return new_mapping;
}
