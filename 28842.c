poppler_annot_mapping_free (PopplerAnnotMapping *mapping)
{
  if (!mapping)
    return;

  if (mapping->annot)
    g_object_unref (mapping->annot);

  g_free (mapping);
}
