void JOIN::drop_unused_derived_keys()
{
  JOIN_TAB *tab;
  for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab; 
       tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
  {
    
    TABLE *tmp_tbl= tab->table;
    if (!tmp_tbl)
      continue;
    if (!tmp_tbl->pos_in_table_list->is_materialized_derived())
      continue;
    if (tmp_tbl->max_keys > 1 && !tab->is_ref_for_hash_join())
      tmp_tbl->use_index(tab->ref.key);
    if (tmp_tbl->s->keys)
    {
      if (tab->ref.key >= 0 && tab->ref.key < MAX_KEY)
        tab->ref.key= 0;
      else
        tmp_tbl->s->keys= 0;
    }
    tab->keys= (key_map) (tmp_tbl->s->keys ? 1 : 0);
  }
}