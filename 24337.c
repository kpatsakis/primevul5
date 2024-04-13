void st_select_lex::remap_tables(TABLE_LIST *derived, table_map map,
                                 uint tablenr, SELECT_LEX *parent_lex)
{
  bool first_table= TRUE;
  TABLE_LIST *tl;
  table_map first_map;
  uint first_tablenr;

  if (derived && derived->table)
  {
    first_map= derived->table->map;
    first_tablenr= derived->table->tablenr;
  }
  else
  {
    first_map= map;
    map<<= 1;
    first_tablenr= tablenr++;
  }
  /*
    Assign table bit/table number.
    To the first table of the subselect the table bit/tablenr of the
    derived table is assigned. The rest of tables are getting bits
    sequentially, starting from the provided table map/tablenr.
  */
  List_iterator<TABLE_LIST> ti(leaf_tables);
  while ((tl= ti++))
  {
    if (first_table)
    {
      first_table= FALSE;
      tl->table->set_table_map(first_map, first_tablenr);
    }
    else
    {
      tl->table->set_table_map(map, tablenr);
      tablenr++;
      map<<= 1;
    }
    SELECT_LEX *old_sl= tl->select_lex;
    tl->select_lex= parent_lex;
    for(TABLE_LIST *emb= tl->embedding;
        emb && emb->select_lex == old_sl;
        emb= emb->embedding)
      emb->select_lex= parent_lex;
  }
}