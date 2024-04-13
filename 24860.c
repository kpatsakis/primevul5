static uint collect_cmp_types(Item **items, uint nitems, bool skip_nulls= FALSE)
{
  uint i;
  uint found_types;
  Item_result left_cmp_type= items[0]->cmp_type();
  DBUG_ASSERT(nitems > 1);
  found_types= 0;
  for (i= 1; i < nitems ; i++)
  {
    if (skip_nulls && items[i]->type() == Item::NULL_ITEM)
      continue; // Skip NULL constant items
    if ((left_cmp_type == ROW_RESULT ||
         items[i]->cmp_type() == ROW_RESULT) &&
        cmp_row_type(items[0], items[i]))
      return 0;
    found_types|= 1U << (uint) item_cmp_type(left_cmp_type, items[i]);
  }
  /*
   Even if all right-hand items are NULLs and we are skipping them all, we need
   at least one type bit in the found_type bitmask.
  */
  if (skip_nulls && !found_types)
    found_types= 1U << (uint) left_cmp_type;
  return found_types;
}