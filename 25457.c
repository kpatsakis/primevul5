longlong Item_func_between::val_int_cmp_datetime()
{
  THD *thd= current_thd;
  longlong value= args[0]->val_datetime_packed(thd), a, b;
  if ((null_value= args[0]->null_value))
    return 0;
  a= args[1]->val_datetime_packed(thd);
  b= args[2]->val_datetime_packed(thd);
  return val_int_cmp_int_finalize(value, a, b);
}