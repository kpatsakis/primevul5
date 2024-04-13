JOIN::reoptimize(Item *added_where, table_map join_tables,
                 Join_plan_state *save_to)
{
  DYNAMIC_ARRAY added_keyuse;
  SARGABLE_PARAM *sargables= 0; /* Used only as a dummy parameter. */
  uint org_keyuse_elements;

  /* Re-run the REF optimizer to take into account the new conditions. */
  if (update_ref_and_keys(thd, &added_keyuse, join_tab, table_count, added_where,
                          ~outer_join, select_lex, &sargables))
  {
    delete_dynamic(&added_keyuse);
    return REOPT_ERROR;
  }

  if (!added_keyuse.elements)
  {
    delete_dynamic(&added_keyuse);
    return REOPT_OLD_PLAN;
  }

  if (save_to)
    save_query_plan(save_to);
  else
    reset_query_plan();

  if (!keyuse.buffer &&
      my_init_dynamic_array(&keyuse, sizeof(KEYUSE), 20, 64,
                            MYF(MY_THREAD_SPECIFIC)))
  {
    delete_dynamic(&added_keyuse);
    return REOPT_ERROR;
  }

  org_keyuse_elements= save_to ? save_to->keyuse.elements : keyuse.elements;
  allocate_dynamic(&keyuse, org_keyuse_elements + added_keyuse.elements);

  /* If needed, add the access methods from the original query plan. */
  if (save_to)
  {
    DBUG_ASSERT(!keyuse.elements);
    keyuse.elements= save_to->keyuse.elements;
    if (size_t e= keyuse.elements)
      memcpy(keyuse.buffer,
             save_to->keyuse.buffer, e * keyuse.size_of_element);
  }

  /* Add the new access methods to the keyuse array. */
  memcpy(keyuse.buffer + keyuse.elements * keyuse.size_of_element,
         added_keyuse.buffer,
         (size_t) added_keyuse.elements * added_keyuse.size_of_element);
  keyuse.elements+= added_keyuse.elements;
  /* added_keyuse contents is copied, and it is no longer needed. */
  delete_dynamic(&added_keyuse);

  if (sort_and_filter_keyuse(thd, &keyuse, true))
    return REOPT_ERROR;
  optimize_keyuse(this, &keyuse);

  if (optimize_semijoin_nests(this, join_tables))
    return REOPT_ERROR;

  /* Re-run the join optimizer to compute a new query plan. */
  if (choose_plan(this, join_tables))
    return REOPT_ERROR;

  return REOPT_NEW_PLAN;
}