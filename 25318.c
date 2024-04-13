Item_func_regex::fix_length_and_dec()
{
  if (Item_bool_func::fix_length_and_dec() ||
      agg_arg_charsets_for_comparison(cmp_collation, args, 2))
    return TRUE;

  re.init(cmp_collation.collation, 0);
  re.fix_owner(this, args[0], args[1]);
  return FALSE;
}