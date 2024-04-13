static void ma_field_extension_init_type_info(MA_MEM_ROOT *memroot,
                                              MA_FIELD_EXTENSION *ext,
                                              const char *ptr, size_t length)
{
  const char *end= ptr + length;
  for ( ;  ptr < end; )
  {
    uint itype= (uchar) *ptr++;
    uint len= (uchar) *ptr++;
    if (ptr + len > end  || len > 127)
      break; /*Badly encoded data*/
    if (itype <= 127 && itype <= MARIADB_FIELD_ATTR_LAST)
      ext->metadata[itype]= ma_const_string_copy_root(memroot, ptr, len);
    ptr+= len;
  }
}