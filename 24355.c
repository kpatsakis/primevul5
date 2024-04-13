void st_select_lex_node::exclude_from_tree()
{
  if ((*prev= next))
    next->prev= prev;
}