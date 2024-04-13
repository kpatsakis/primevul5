bool cmp_item_row::prepare_comparators(THD *thd, const char *funcname,
                                       const Item_args *args, uint level)
{
  DBUG_EXECUTE_IF("cmp_item",
                  push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                  ER_UNKNOWN_ERROR, "DBUG: %sROW(%d args) level=%d",
                  String_space(level).c_ptr(),
                  args->argument_count(), level););
  DBUG_ASSERT(args->argument_count() > 0);
  if (alloc_comparators(thd, args->arguments()[0]->cols()))
    return true;
  DBUG_ASSERT(n == args->arguments()[0]->cols());
  for (uint col= 0; col < n; col++)
  {
    Item_args tmp;
    Type_handler_hybrid_field_type cmp;

    if (tmp.alloc_and_extract_row_elements(thd, args, col) ||
        aggregate_row_elements_for_comparison(thd, &cmp, &tmp,
                                              funcname, col, level + 1))
      return true;

    /*
      There is a legacy bug (MDEV-11511) in the code below,
      which should be fixed eventually.
      When performing:
       (predicant0,predicant1) IN ((value00,value01),(value10,value11))
      It uses only the data type and the collation of the predicant
      elements only. It should be fixed to take into account the data type and
      the collation for all elements at the N-th positions of the
      predicate and all values:
      - predicate0, value00, value01
      - predicate1, value10, value11
    */
    Item *item0= args->arguments()[0]->element_index(col);
    CHARSET_INFO *collation= item0->collation.collation;
    if (!(comparators[col]= cmp.type_handler()->make_cmp_item(thd, collation)))
      return true;
    if (cmp.type_handler() == &type_handler_row)
    {
      // Prepare comparators for ROW elements recursively
      cmp_item_row *row= static_cast<cmp_item_row*>(comparators[col]);
      if (row->prepare_comparators(thd, funcname, &tmp, level + 1))
        return true;
    }
  }
  return false;
}