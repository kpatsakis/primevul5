Item_func_regexp_instr::fix_length_and_dec()
{
  if (agg_arg_charsets_for_comparison(cmp_collation, args, 2))
    return TRUE;

  re.init(cmp_collation.collation, 0);
  re.fix_owner(this, args[0], args[1]);
  max_length= MY_INT32_NUM_DECIMAL_DIGITS; // See also Item_func_locate
  return FALSE;
}