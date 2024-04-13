join_tab_cmp_straight(const void *dummy, const void* ptr1, const void* ptr2)
{
  JOIN_TAB *jt1= *(JOIN_TAB**) ptr1;
  JOIN_TAB *jt2= *(JOIN_TAB**) ptr2;

  /*
    We don't do subquery flattening if the parent or child select has
    STRAIGHT_JOIN modifier. It is complicated to implement and the semantics
    is hardly useful.
  */
  DBUG_ASSERT(!jt1->emb_sj_nest);
  DBUG_ASSERT(!jt2->emb_sj_nest);

  int cmp;
  if ((cmp= compare_embedding_subqueries(jt1, jt2)) != 0)
    return cmp;

  if (jt1->dependent & jt2->table->map)
    return 1;
  if (jt2->dependent & jt1->table->map)
    return -1;
  return jt1 > jt2 ? 1 : (jt1 < jt2 ? -1 : 0);
}