Item_equal::Item_equal(THD *thd, Item_equal *item_equal):
  Item_bool_func(thd), eval_item(0), cond_false(0), cond_true(0),
  context_field(NULL), link_equal_fields(FALSE),
  m_compare_handler(item_equal->m_compare_handler),
  m_compare_collation(item_equal->m_compare_collation)
{
  const_item_cache= 0;
  List_iterator_fast<Item> li(item_equal->equal_items);
  Item *item;
  while ((item= li++))
  {
    equal_items.push_back(item, thd->mem_root);
  }
  with_const= item_equal->with_const;
  cond_false= item_equal->cond_false;
  upper_levels= item_equal->upper_levels;
}