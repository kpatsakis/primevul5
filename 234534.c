Item_bool_func2::remove_eq_conds(THD *thd, Item::cond_result *cond_value,
                                 bool top_level_arg)
{
  if (const_item() && !is_expensive())
  {
    *cond_value= eval_const_cond() ? Item::COND_TRUE : Item::COND_FALSE;
    return (COND*) 0;
  }
  if ((*cond_value= eq_cmp_result()) != Item::COND_OK)
  {
    if (args[0]->eq(args[1], true))
    {
      if (!args[0]->maybe_null || functype() == Item_func::EQUAL_FUNC)
        return (COND*) 0;                       // Compare of identical items
    }
  }
  *cond_value= Item::COND_OK;
  return this;                                  // Point at next and level
}