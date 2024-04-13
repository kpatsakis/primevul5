bool Arg_comparator::set_cmp_func_for_row_arguments()
{
  uint n= (*a)->cols();
  if (n != (*b)->cols())
  {
    my_error(ER_OPERAND_COLUMNS, MYF(0), n);
    comparators= 0;
    return true;
  }
  if (!(comparators= new Arg_comparator[n]))
    return true;
  for (uint i=0; i < n; i++)
  {
    if ((*a)->element_index(i)->cols() != (*b)->element_index(i)->cols())
    {
      my_error(ER_OPERAND_COLUMNS, MYF(0), (*a)->element_index(i)->cols());
      return true;
    }
    if (comparators[i].set_cmp_func(owner, (*a)->addr(i),
                                           (*b)->addr(i), set_null))
      return true;
  }
  return false;
}