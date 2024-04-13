bool Item_func_regex::fix_fields(THD *thd, Item **ref)
{
  re.set_recursion_limit(thd);
  return Item_bool_func::fix_fields(thd, ref);
}