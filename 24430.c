void st_select_lex::set_index_hint_type(enum index_hint_type type_arg,
                                        index_clause_map clause)
{ 
  current_index_hint_type= type_arg;
  current_index_hint_clause= clause;
}