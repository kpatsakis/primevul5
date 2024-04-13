st_select_lex_node *st_select_lex_node:: insert_chain_before(
				         st_select_lex_node **ptr_pos_to_insert,
                                         st_select_lex_node *end_chain_node)
{
  end_chain_node->link_next= *ptr_pos_to_insert;
  (*ptr_pos_to_insert)->link_prev= &end_chain_node->link_next;
  this->link_prev= ptr_pos_to_insert;
  return this;
}