int st_select_lex_unit::save_union_explain_part2(Explain_query *output)
{
  Explain_union *eu= output->get_union(first_select()->select_number);
  if (fake_select_lex)
  {
    for (SELECT_LEX_UNIT *unit= fake_select_lex->first_inner_unit(); 
         unit; unit= unit->next_unit())
    {
      if (!(unit->item && unit->item->eliminated))
      {
        eu->add_child(unit->first_select()->select_number);
      }
    }
    fake_select_lex->join->explain= &eu->fake_select_lex_explain;
  }
  return 0;
}