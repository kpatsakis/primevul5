static bool sort_and_filter_keyuse(THD *thd, DYNAMIC_ARRAY *keyuse, 
                                   bool skip_unprefixed_keyparts)
{
  KEYUSE key_end, *prev, *save_pos, *use;
  uint found_eq_constant, i;

  DBUG_ASSERT(keyuse->elements);

  my_qsort(keyuse->buffer, keyuse->elements, sizeof(KEYUSE),
           (qsort_cmp) sort_keyuse);

  bzero((char*) &key_end, sizeof(key_end));    /* Add for easy testing */
  if (insert_dynamic(keyuse, (uchar*) &key_end))
    return TRUE;

  if (optimizer_flag(thd, OPTIMIZER_SWITCH_DERIVED_WITH_KEYS))
    generate_derived_keys(keyuse);

  use= save_pos= dynamic_element(keyuse,0,KEYUSE*);
  prev= &key_end;
  found_eq_constant= 0;
  for (i=0 ; i < keyuse->elements-1 ; i++,use++)
  {
    if (!use->is_for_hash_join())
    {
      if (!(use->used_tables & ~OUTER_REF_TABLE_BIT) && 
          use->optimize != KEY_OPTIMIZE_REF_OR_NULL)
        use->table->const_key_parts[use->key]|= use->keypart_map;
      if (use->keypart != FT_KEYPART)
      {
        if (use->key == prev->key && use->table == prev->table)
        {
          if ((prev->keypart+1 < use->keypart && skip_unprefixed_keyparts) ||
              (prev->keypart == use->keypart && found_eq_constant))
            continue;				/* remove */
        }
        else if (use->keypart != 0 && skip_unprefixed_keyparts)
          continue; /* remove - first found must be 0 */
      }

      prev= use;
      found_eq_constant= !use->used_tables;
      use->table->reginfo.join_tab->checked_keys.set_bit(use->key);
    }
    /*
      Old gcc used a memcpy(), which is undefined if save_pos==use:
      http://gcc.gnu.org/bugzilla/show_bug.cgi?id=19410
      http://gcc.gnu.org/bugzilla/show_bug.cgi?id=39480
      This also disables a valgrind warning, so better to have the test.
    */
    if (save_pos != use)
      *save_pos= *use;
    /* Save ptr to first use */
    if (!use->table->reginfo.join_tab->keyuse)
      use->table->reginfo.join_tab->keyuse= save_pos;
    save_pos++;
  }
  i= (uint) (save_pos-(KEYUSE*) keyuse->buffer);
  (void) set_dynamic(keyuse,(uchar*) &key_end,i);
  keyuse->elements= i;

  return FALSE;
}