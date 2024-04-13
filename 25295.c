inline Item* get_item_copy (THD *thd, T* item)
{
  Item *copy= new (get_thd_memroot(thd)) T(*item);
  if (likely(copy))
    copy->register_in(thd);
  return copy;
}	