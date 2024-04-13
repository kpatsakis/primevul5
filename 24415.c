void SELECT_LEX::mark_const_derived(bool empty)
{
  TABLE_LIST *derived= master_unit()->derived;
  /* join == NULL in  DELETE ... RETURNING */
  if (!(join && join->thd->lex->describe) && derived)
  {
    if (!empty)
      increase_derived_records(1);
    if (!master_unit()->is_union() && !derived->is_merged_derived())
      derived->fill_me= TRUE;
  }
}