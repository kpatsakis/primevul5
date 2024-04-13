static Item **get_sargable_cond(JOIN *join, TABLE *table)
{
  Item **retval;
  if (table->pos_in_table_list->on_expr)
  {
    /*
      This is an inner table from a single-table LEFT JOIN, "t1 LEFT JOIN
      t2 ON cond". Use the condition cond.
    */
    retval= &table->pos_in_table_list->on_expr;
  }
  else if (table->pos_in_table_list->embedding &&
           !table->pos_in_table_list->embedding->sj_on_expr)
  {
    /*
      This is the inner side of a multi-table outer join. Use the
      appropriate ON expression.
    */
    retval= &(table->pos_in_table_list->embedding->on_expr);
  }
  else
  {
    /* The table is not inner wrt some LEFT JOIN. Use the WHERE clause */
    retval= &join->conds;
  }
  return retval;
}