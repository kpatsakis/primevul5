bool st_select_lex::handle_derived(LEX *lex, uint phases)
{
  return lex->handle_list_of_derived(table_list.first, phases);
}