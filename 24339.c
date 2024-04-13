void st_select_lex_node::add_slave(st_select_lex_node *slave_arg)
{
  for (; slave; slave= slave->next)
    if (slave == slave_arg)
      return;

  if (slave)
  {
    st_select_lex_node *slave_arg_slave= slave_arg->slave;
    /* Insert in the front of list of slaves if any. */
    slave_arg->include_neighbour(slave);
    /* include_neighbour() sets slave_arg->slave=0, restore it. */
    slave_arg->slave= slave_arg_slave;
    /* Count on include_neighbour() setting the master. */
    DBUG_ASSERT(slave_arg->master == this);
  }
  else
  {
    slave= slave_arg;
    slave_arg->master= this;
  }
}