void st_select_lex::set_unique_exclude()
{
  exclude_from_table_unique_test= TRUE;
  for (SELECT_LEX_UNIT *unit= first_inner_unit();
       unit;
       unit= unit->next_unit())
  {
    if (unit->derived && unit->derived->is_view_or_derived())
    {
      for (SELECT_LEX *sl= unit->first_select(); sl; sl= sl->next_select())
        sl->set_unique_exclude();
    }
  }
}