inline Item* get_item_copy (THD *thd, MEM_ROOT *mem_root, T* item)
{
  Item *copy= new (mem_root) T(*item);
  copy->register_in(thd);
  return copy;
}	