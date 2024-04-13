MA_FIELD_EXTENSION *ma_field_extension_deep_dup(MA_MEM_ROOT *memroot,
                                                const MA_FIELD_EXTENSION *from)
{
  MA_FIELD_EXTENSION *ext= new_ma_field_extension(memroot);
  uint i;
  if (!ext)
    return NULL;
  for (i= 0; i < MARIADB_FIELD_ATTR_LAST; i++)
  {
    if (from->metadata[i].str)
      ext->metadata[i]= ma_const_string_copy_root(memroot,
                                                  from->metadata[i].str,
                                                  from->metadata[i].length);
  }
  return ext;
}