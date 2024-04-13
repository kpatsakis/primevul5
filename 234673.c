join_tab_cmp(const void *dummy, const void* ptr1, const void* ptr2)
{
  JOIN_TAB *jt1= *(JOIN_TAB**) ptr1;
  JOIN_TAB *jt2= *(JOIN_TAB**) ptr2;
  int cmp;

  if ((cmp= compare_embedding_subqueries(jt1, jt2)) != 0)
    return cmp;
  /*
    After that,
    take care about ordering imposed by LEFT JOIN constraints,
    possible [eq]ref accesses, and numbers of matching records in the table.
  */
  if (jt1->dependent & jt2->table->map)
    return 1;
  if (jt2->dependent & jt1->table->map)
    return -1;  
  if (jt1->found_records > jt2->found_records)
    return 1;
  if (jt1->found_records < jt2->found_records)
    return -1; 
  return jt1 > jt2 ? 1 : (jt1 < jt2 ? -1 : 0);
}