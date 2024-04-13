Item *Item_in_optimizer::expr_cache_insert_transformer(THD *thd, uchar *unused)
{
  DBUG_ENTER("Item_in_optimizer::expr_cache_insert_transformer");
  DBUG_ASSERT(fixed);

  if (invisible_mode())
    DBUG_RETURN(this);

  if (expr_cache)
    DBUG_RETURN(expr_cache);

  if (args[1]->expr_cache_is_needed(thd) &&
      (expr_cache= set_expr_cache(thd)))
    DBUG_RETURN(expr_cache);

  DBUG_RETURN(this);
}