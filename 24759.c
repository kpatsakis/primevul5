longlong Item_func_in::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if (array)
  {
    bool tmp=array->find(args[0]);
    /*
      NULL on left -> UNKNOWN.
      Found no match, and NULL on right -> UNKNOWN.
      NULL on right can never give a match, as it is not stored in
      array.
      See also the 'bisection_possible' variable in fix_length_and_dec().
    */
    null_value=args[0]->null_value || (!tmp && have_null);
    return (longlong) (!null_value && tmp != negated);
  }

  if ((null_value= args[0]->real_item()->type() == NULL_ITEM))
    return 0;

  null_value= have_null;
  uint idx;
  if (!Predicant_to_list_comparator::cmp(this, &idx, &null_value))
  {
    null_value= false;
    return (longlong) (!negated);
  }
  return (longlong) (!null_value && negated);
}