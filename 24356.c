st_select_lex* st_select_lex::outer_select()
{
  return (st_select_lex*) master->get_master();
}