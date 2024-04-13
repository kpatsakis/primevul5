void *Virtual_tmp_table::operator new(size_t size, THD *thd) throw()
{
  return (Virtual_tmp_table *) alloc_root(thd->mem_root, size);
}