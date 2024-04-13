Item_equal::Item_equal(THD *thd, const Type_handler *handler,
                       Item *f1, Item *f2, bool with_const_item):
  Item_bool_func(thd), eval_item(0), cond_false(0), cond_true(0),
  context_field(NULL), link_equal_fields(FALSE),
  m_compare_handler(handler),
  m_compare_collation(f2->collation.collation)
{
  const_item_cache= 0;
  with_const= with_const_item;
  equal_items.push_back(f1, thd->mem_root);
  equal_items.push_back(f2, thd->mem_root);
  upper_levels= NULL;
}