void st_select_lex_node::move_as_slave(st_select_lex_node *new_master)
{
  exclude_from_tree();
  if (new_master->slave)
  {
    st_select_lex_node *curr= new_master->slave;
    for ( ; curr->next ; curr= curr->next) ;
    prev= &curr->next;
  }
  else
    prev= &new_master->slave;
  *prev= this;
  next= 0;
  master= new_master;
}