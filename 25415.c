bool cmp_item_row::
      aggregate_row_elements_for_comparison(THD *thd,
                                            Type_handler_hybrid_field_type *cmp,
                                            Item_args *tmp,
                                            const char *funcname,
                                            uint col,
                                            uint level)
{
  DBUG_EXECUTE_IF("cmp_item",
  {
    for (uint i= 0 ; i < tmp->argument_count(); i++)
    {
      Item *arg= tmp->arguments()[i];
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                          ER_UNKNOWN_ERROR, "DBUG: %s[%d,%d] handler=%s",
                          String_space(level).c_ptr(), col, i,
                          arg->type_handler()->name().ptr());
    }
  }
  );
  bool err= cmp->aggregate_for_comparison(funcname, tmp->arguments(),
                                          tmp->argument_count(), true);
  DBUG_EXECUTE_IF("cmp_item",
  {
    if (!err)
      push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                          ER_UNKNOWN_ERROR, "DBUG: %s=> handler=%s",
                          String_space(level).c_ptr(),
                          cmp->type_handler()->name().ptr());
  }
  );
  return err;
}