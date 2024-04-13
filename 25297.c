bool Item_func_in::value_list_convert_const_to_int(THD *thd)
{
  if (args[0]->real_item()->type() == FIELD_ITEM &&
      !thd->lex->is_view_context_analysis())
  {
    Item_field *field_item= (Item_field*) (args[0]->real_item());
    if (field_item->field_type() == MYSQL_TYPE_LONGLONG ||
        field_item->field_type() == MYSQL_TYPE_YEAR)
    {
      bool all_converted= true;
      Item **arg, **arg_end;
      for (arg=args+1, arg_end=args+arg_count; arg != arg_end ; arg++)
      {
          /*
            Explicit NULLs should not affect data cmp_type resolution:
            - we ignore NULLs when calling collect_cmp_type()
            - we ignore NULLs here
            So this expression:
              year_column IN (DATE'2001-01-01', NULL)
            switches from TIME_RESULT to INT_RESULT.
          */
          if (arg[0]->type() != Item::NULL_ITEM &&
              !convert_const_to_int(thd, field_item, &arg[0]))
           all_converted= false;
      }
      if (all_converted)
        m_comparator.set_handler(&type_handler_longlong);
    }
  }
  return thd->is_fatal_error; // Catch errrors in convert_const_to_int
}