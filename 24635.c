Item_func_in::fix_fields(THD *thd, Item **ref)
{

  if (Item_func_opt_neg::fix_fields(thd, ref))
    return 1;

  return 0;
}