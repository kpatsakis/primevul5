resize_property_list(int new_size, const OnigCodePoint*** plist, int* psize)
{
  size_t size;
  const OnigCodePoint **list = *plist;

  size = sizeof(OnigCodePoint*) * new_size;
  if (IS_NULL(list)) {
    list = (const OnigCodePoint** )xmalloc(size);
  }
  else {
    list = (const OnigCodePoint** )xrealloc((void* )list, size);
  }

  if (IS_NULL(list)) return ONIGERR_MEMORY;

  *plist = list;
  *psize = new_size;

  return 0;
}