poppler_form_field_mapping_free (PopplerFormFieldMapping *mapping)
{
  if (!mapping)
    return;

  if (mapping->field)
    g_object_unref (mapping->field);
  
  g_free (mapping);
}
