  bool alloc_and_extract_row_elements(THD *thd, const Item_args *rows, uint pos)
  {
    DBUG_ASSERT(rows->argument_count() > 0);
    DBUG_ASSERT(rows->arguments()[0]->cols() > pos);
    if (alloc_arguments(thd, rows->argument_count()))
      return true;
    for (uint i= 0; i < rows->argument_count(); i++)
    {
      DBUG_ASSERT(rows->arguments()[0]->cols() == rows->arguments()[i]->cols());
      Item *arg= rows->arguments()[i]->element_index(pos);
      add_argument(arg);
    }
    DBUG_ASSERT(argument_count() == rows->argument_count());
    return false;
  }