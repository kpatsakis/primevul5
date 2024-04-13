static void update_const_equal_items(THD *thd, COND *cond, JOIN_TAB *tab,
                                     bool const_key)
{
  if (!(cond->used_tables() & tab->table->map))
    return;

  if (cond->type() == Item::COND_ITEM)
  {
    List<Item> *cond_list= ((Item_cond*) cond)->argument_list(); 
    List_iterator_fast<Item> li(*cond_list);
    Item *item;
    while ((item= li++))
      update_const_equal_items(thd, item, tab,
                               (((Item_cond*) cond)->top_level() &&
                                ((Item_cond*) cond)->functype() ==
                                Item_func::COND_AND_FUNC));
  }
  else if (cond->type() == Item::FUNC_ITEM && 
           ((Item_cond*) cond)->functype() == Item_func::MULT_EQUAL_FUNC)
  {
    Item_equal *item_equal= (Item_equal *) cond;
    bool contained_const= item_equal->get_const() != NULL;
    item_equal->update_const(thd);
    if (!contained_const && item_equal->get_const())
    {
      /* Update keys for range analysis */
      Item_equal_fields_iterator it(*item_equal);
      while (it++)
      {
        Field *field= it.get_curr_field();
        JOIN_TAB *stat= field->table->reginfo.join_tab;
        key_map possible_keys= field->key_start;
        possible_keys.intersect(field->table->keys_in_use_for_query);
        stat[0].const_keys.merge(possible_keys);

        /*
          For each field in the multiple equality (for which we know that it 
          is a constant) we have to find its corresponding key part, and set 
          that key part in const_key_parts.
        */  
        if (!possible_keys.is_clear_all())
        {
          TABLE *field_tab= field->table;
          KEYUSE *use;
          for (use= stat->keyuse; use && use->table == field_tab; use++)
            if (const_key &&
                !use->is_for_hash_join() && possible_keys.is_set(use->key) && 
                field_tab->key_info[use->key].key_part[use->keypart].field ==
                field)
              field_tab->const_key_parts[use->key]|= use->keypart_map;
        }
      }
    }
  }
}