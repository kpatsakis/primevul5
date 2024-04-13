bool st_select_lex::save_prep_leaf_tables(THD *thd)
{
  if (prep_leaf_list_state == SAVED)
    return FALSE;

  List_iterator_fast<TABLE_LIST> li(leaf_tables);
  TABLE_LIST *table;

  /*
    Check that the SELECT_LEX was really prepared and so tables are setup.

    It can be subquery in SET clause of UPDATE which was not prepared yet, so
    its tables are not yet setup and ready for storing.
  */
  if (prep_leaf_list_state != READY)
    return FALSE;

  while ((table= li++))
  {
    if (leaf_tables_prep.push_back(table))
      return TRUE;
  }
  prep_leaf_list_state= SAVED;
  for (SELECT_LEX_UNIT *u= first_inner_unit(); u; u= u->next_unit())
  {
    for (SELECT_LEX *sl= u->first_select(); sl; sl= sl->next_select())
    {
      if (sl->save_prep_leaf_tables(thd))
        return TRUE;
    }
  }

  return FALSE;
}