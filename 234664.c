get_sort_by_table(ORDER *a,ORDER *b, List<TABLE_LIST> &tables, 
                  table_map const_tables)
{
  TABLE_LIST *table;
  List_iterator<TABLE_LIST> ti(tables);
  table_map map= (table_map) 0;
  DBUG_ENTER("get_sort_by_table");

  if (!a)
    a=b;					// Only one need to be given
  else if (!b)
    b=a;

  for (; a && b; a=a->next,b=b->next)
  {
    /* Skip elements of a that are constant */
    while (!((*a->item)->used_tables() & ~const_tables))
    {
      if (!(a= a->next))
        break;
    }

    /* Skip elements of b that are constant */
    while (!((*b->item)->used_tables() & ~const_tables))
    {
      if (!(b= b->next))
        break;
    }

    if (!a || !b)
      break;

    if (!(*a->item)->eq(*b->item,1))
      DBUG_RETURN(0);
    map|=a->item[0]->used_tables();
  }
  if (!map || (map & (RAND_TABLE_BIT | OUTER_REF_TABLE_BIT)))
    DBUG_RETURN(0);

  map&= ~const_tables;
  while ((table= ti++) && !(map & table->table->map)) ;
  if (map != table->table->map)
    DBUG_RETURN(0);				// More than one table
  DBUG_PRINT("exit",("sort by table: %d",table->table->tablenr));
  DBUG_RETURN(table->table);
}