longlong Item_func_between::val_int_cmp_native()
{
  THD *thd= current_thd;
  const Type_handler *h= m_comparator.type_handler();
  NativeBuffer<STRING_BUFFER_USUAL_SIZE> value, a, b;
  if (val_native_with_conversion_from_item(thd, args[0], &value, h))
    return 0;
  bool ra= args[1]->val_native_with_conversion(thd, &a, h);
  bool rb= args[2]->val_native_with_conversion(thd, &b, h);
  if (!ra && !rb)
    return (longlong)
      ((h->cmp_native(value, a) >= 0 &&
        h->cmp_native(value, b) <= 0) != negated);
  if (ra && rb)
    null_value= true;
  else if (ra)
    null_value= h->cmp_native(value, b) <= 0;
  else
    null_value= h->cmp_native(value, a) >= 0;
  return (longlong) (!null_value && negated);
}