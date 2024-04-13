bool Item_func_regexp_instr::fix_fields(THD *thd, Item **ref)
{
  re.set_recursion_limit(thd);
  return Item_int_func::fix_fields(thd, ref);
}