join_tab_cmp_embedded_first(const void *emb,  const void* ptr1, const void* ptr2)
{
  const TABLE_LIST *emb_nest= (TABLE_LIST*) emb;
  JOIN_TAB *jt1= *(JOIN_TAB**) ptr1;
  JOIN_TAB *jt2= *(JOIN_TAB**) ptr2;

  if (jt1->emb_sj_nest == emb_nest && jt2->emb_sj_nest != emb_nest)
    return -1;
  if (jt1->emb_sj_nest != emb_nest && jt2->emb_sj_nest == emb_nest)
    return 1;

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