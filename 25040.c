bool cmp_item_row::alloc_comparators(THD *thd, uint cols)
{
  if (comparators)
  {
    DBUG_ASSERT(cols == n);
    return false;
  }
  return
    !(comparators= (cmp_item **) thd->calloc(sizeof(cmp_item *) * (n= cols)));
}