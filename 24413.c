void st_select_lex_node::exclude()
{
  /* exclude from global list */
  fast_exclude();
  /* exclude from other structures */
  exclude_from_tree();
  /* 
     We do not need following statements, because prev pointer of first 
     list element point to master->slave
     if (master->slave == this)
       master->slave= next;
  */
}