Item::remove_eq_conds(THD *thd, Item::cond_result *cond_value, bool top_level_arg)
{
  if (const_item() && !is_expensive())
  {
    *cond_value= eval_const_cond() ? Item::COND_TRUE : Item::COND_FALSE;
    return (COND*) 0;
  }
  *cond_value= Item::COND_OK;
  return this;                                        // Point at next and level
}