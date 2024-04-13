static void print_join(THD *thd,
                       table_map eliminated_tables,
                       String *str,
                       List<TABLE_LIST> *tables,
                       enum_query_type query_type)
{
  /* List is reversed => we should reverse it before using */
  List_iterator_fast<TABLE_LIST> ti(*tables);
  TABLE_LIST **table;
  DBUG_ENTER("print_join");

  /*
    If the QT_NO_DATA_EXPANSION flag is specified, we print the
    original table list, including constant tables that have been
    optimized away, as the constant tables may be referenced in the
    expression printed by Item_field::print() when this flag is given.
    Otherwise, only non-const tables are printed.

    Example:

    Original SQL:
    select * from (select 1) t

    Printed without QT_NO_DATA_EXPANSION:
    select '1' AS `1` from dual

    Printed with QT_NO_DATA_EXPANSION:
    select `t`.`1` from (select 1 AS `1`) `t`
  */
  const bool print_const_tables= (query_type & QT_NO_DATA_EXPANSION);
  size_t tables_to_print= 0;

  for (TABLE_LIST *t= ti++; t ; t= ti++)
  {
    /* See comment in print_table_array() about the second condition */
    if (print_const_tables || !t->optimized_away)
      if (!is_eliminated_table(eliminated_tables, t))
        tables_to_print++;
  }
  if (tables_to_print == 0)
  {
    str->append(STRING_WITH_LEN("dual"));
    DBUG_VOID_RETURN;                   // all tables were optimized away
  }
  ti.rewind();

  if (!(table= static_cast<TABLE_LIST **>(thd->alloc(sizeof(TABLE_LIST*) *
                                                     tables_to_print))))
    DBUG_VOID_RETURN;                   // out of memory

  TABLE_LIST *tmp, **t= table + (tables_to_print - 1);
  while ((tmp= ti++))
  {
    if (tmp->optimized_away && !print_const_tables)
      continue;
    if (is_eliminated_table(eliminated_tables, tmp))
      continue;
    *t--= tmp;
  }

  DBUG_ASSERT(tables->elements >= 1);
  /*
    Assert that the first table in the list isn't eliminated. This comes from
    the fact that the first table can't be inner table of an outer join.
  */
  DBUG_ASSERT(!eliminated_tables || 
              !(((*table)->table && ((*table)->table->map & eliminated_tables)) ||
                ((*table)->nested_join && !((*table)->nested_join->used_tables &
                                           ~eliminated_tables))));
  /* 
    If the first table is a semi-join nest, swap it with something that is
    not a semi-join nest.
  */
  if ((*table)->sj_inner_tables)
  {
    TABLE_LIST **end= table + tables_to_print;
    for (TABLE_LIST **t2= table; t2!=end; t2++)
    {
      if (!(*t2)->sj_inner_tables)
      {
        tmp= *t2;
        *t2= *table;
        *table= tmp;
        break;
      }
    }
  }
  print_table_array(thd, eliminated_tables, str, table, 
                    table +  tables_to_print, query_type);
  DBUG_VOID_RETURN;
}