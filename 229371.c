MA_FIELD_EXTENSION *new_ma_field_extension(MA_MEM_ROOT *memroot)
{
  MA_FIELD_EXTENSION *ext= ma_alloc_root(memroot, sizeof(MA_FIELD_EXTENSION));
  if (ext)
    memset((void *) ext, 0, sizeof(*ext));
  return ext;
}