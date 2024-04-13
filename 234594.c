COND *Item_equal::build_equal_items(THD *thd, COND_EQUAL *inherited,
                                    bool link_item_fields,
                                    COND_EQUAL **cond_equal_ref)
{
  COND *cond= Item_func::build_equal_items(thd, inherited, link_item_fields,
                                           cond_equal_ref);
  if (cond_equal_ref)
    *cond_equal_ref= new (thd->mem_root) COND_EQUAL(this, thd->mem_root);
  return cond;
}