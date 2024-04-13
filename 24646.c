longlong Item_func_between::val_int_cmp_decimal()
{
  VDec dec(args[0]);
  if ((null_value= dec.is_null()))
    return 0;					/* purecov: inspected */
  VDec a_dec(args[1]), b_dec(args[2]);
  if (!a_dec.is_null() && !b_dec.is_null())
    return (longlong) ((dec.cmp(a_dec) >= 0 &&
                        dec.cmp(b_dec) <= 0) != negated);
  if (a_dec.is_null() && b_dec.is_null())
    null_value= true;
  else if (a_dec.is_null())
    null_value= (dec.cmp(b_dec) <= 0);
  else
    null_value= (dec.cmp(a_dec) >= 0);
  return (longlong) (!null_value && negated);
}