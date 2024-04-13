void SELECT_LEX::mark_as_belong_to_derived(TABLE_LIST *derived)
{
  /* Mark tables as belonging to this DT */
  TABLE_LIST *tl;
  List_iterator<TABLE_LIST> ti(leaf_tables);
  while ((tl= ti++))
    tl->belong_to_derived= derived;
}