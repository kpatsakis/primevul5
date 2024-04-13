JOIN_TAB *first_linear_tab(JOIN *join,
                           enum enum_with_bush_roots include_bush_roots,
                           enum enum_with_const_tables const_tbls)
{
  JOIN_TAB *first= join->join_tab;

  if (!first)
    return NULL;

  if (const_tbls == WITHOUT_CONST_TABLES)
    first+= join->const_tables;

  if (first >= join->join_tab + join->top_join_tab_count)
    return NULL; /* All are const tables */

  if (first->bush_children && include_bush_roots == WITHOUT_BUSH_ROOTS)
  {
    /* This JOIN_TAB is a SJM nest; Start from first table in nest */
    return first->bush_children->start;
  }

  return first;
}