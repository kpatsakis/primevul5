int cmp_item_row::cmp(Item *arg)
{
  arg->null_value= 0;
  if (arg->cols() != n)
  {
    my_error(ER_OPERAND_COLUMNS, MYF(0), n);
    return 1;
  }
  bool was_null= 0;
  arg->bring_value();
  for (uint i=0; i < n; i++)
  {
    const int rc= comparators[i]->cmp(arg->element_index(i));
    switch (rc)
    {
    case UNKNOWN:
      was_null= true;
      break;
    case TRUE:
      return TRUE;
    case FALSE:
      break;                                    // elements #i are equal
    }
    arg->null_value|= arg->element_index(i)->null_value;
  }
  return was_null ? UNKNOWN : FALSE;
}