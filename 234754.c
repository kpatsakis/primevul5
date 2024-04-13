COND *Item_func::build_equal_items(THD *thd, COND_EQUAL *inherited,
                                   bool link_item_fields,
                                   COND_EQUAL **cond_equal_ref)
{
  /* 
    For each field reference in cond, not from equal item predicates,
    set a pointer to the multiple equality it belongs to (if there is any)
    as soon the field is not of a string type or the field reference is
    an argument of a comparison predicate. 
  */ 
  COND *cond= propagate_equal_fields(thd, Context_boolean(), inherited);
  cond->update_used_tables();
  DBUG_ASSERT(cond == this);
  DBUG_ASSERT(!cond_equal_ref || !cond_equal_ref[0]);
  return cond;
}