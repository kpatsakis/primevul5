void JOIN_TAB::save_explain_data(Explain_table_access *eta,
                                 table_map prefix_tables, 
                                 bool distinct_arg, JOIN_TAB *first_top_tab)
{
  int quick_type;
  CHARSET_INFO *cs= system_charset_info;
  THD *thd=      join->thd;
  TABLE_LIST *table_list= table->pos_in_table_list;
  QUICK_SELECT_I *cur_quick= NULL;
  my_bool key_read;
  char table_name_buffer[SAFE_NAME_LEN];
  KEY *key_info= 0;
  uint key_len= 0;
  quick_type= -1;

  explain_plan= eta;
  eta->key.clear();
  eta->quick_info= NULL;

  SQL_SELECT *tab_select;
  /* 
    We assume that if this table does pre-sorting, then it doesn't do filtering
    with SQL_SELECT.
  */
  DBUG_ASSERT(!(select && filesort));
  tab_select= (filesort)? filesort->select : select;

  if (filesort)
  {
    eta->pre_join_sort= new (thd->mem_root) Explain_aggr_filesort(thd->mem_root,
                                                  thd->lex->analyze_stmt,
                                                  filesort);
  }
  
  tracker= &eta->tracker;
  jbuf_tracker= &eta->jbuf_tracker;

  /* Enable the table access time tracker only for "ANALYZE stmt" */
  if (thd->lex->analyze_stmt)
    table->file->set_time_tracker(&eta->op_tracker);

  /* No need to save id and select_type here, they are kept in Explain_select */

  /* table */
  if (table->derived_select_number)
  {
    /* Derived table name generation */
    int len= my_snprintf(table_name_buffer, sizeof(table_name_buffer)-1,
                         "<derived%u>",
                         table->derived_select_number);
    eta->table_name.copy(table_name_buffer, len, cs);
  }
  else if (bush_children)
  {
    JOIN_TAB *ctab= bush_children->start;
    /* table */
    int len= my_snprintf(table_name_buffer, 
                         sizeof(table_name_buffer)-1,
                         "<subquery%d>", 
                         ctab->emb_sj_nest->sj_subq_pred->get_identifier());
    eta->table_name.copy(table_name_buffer, len, cs);
  }
  else
  {
    TABLE_LIST *real_table= table->pos_in_table_list;
    /*
      When multi-table UPDATE/DELETE does updates/deletes to a VIEW, the view
      is merged in a certain particular way (grep for DT_MERGE_FOR_INSERT).

      As a result, view's underlying tables have $tbl->pos_in_table_list={view}.
      We don't want to print view name in EXPLAIN, we want underlying table's
      alias (like specified in the view definition).
    */
    if (real_table->merged_for_insert)
    {
      TABLE_LIST *view_child= real_table->view->select_lex.table_list.first;
      for (;view_child; view_child= view_child->next_local)
      {
        if (view_child->table == table)
        {
          real_table= view_child;
          break;
        }
      }
    }
    eta->table_name.copy(real_table->alias, strlen(real_table->alias), cs);
  }

  /* "partitions" column */
  {
#ifdef WITH_PARTITION_STORAGE_ENGINE
    partition_info *part_info;
    if (!table->derived_select_number && 
        (part_info= table->part_info))
    { //TODO: all thd->mem_root here should be fixed
      make_used_partitions_str(thd->mem_root, part_info, &eta->used_partitions,
                               eta->used_partitions_list);
      eta->used_partitions_set= true;
    }
    else
      eta->used_partitions_set= false;
#else
    /* just produce empty column if partitioning is not compiled in */
    eta->used_partitions_set= false;
#endif
  }

  /* "type" column */
  enum join_type tab_type= type;
  if ((type == JT_ALL || type == JT_HASH) &&
       tab_select && tab_select->quick && use_quick != 2)
  {
    cur_quick= tab_select->quick;
    quick_type= cur_quick->get_type();
    if ((quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_MERGE) ||
        (quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_INTERSECT) ||
        (quick_type == QUICK_SELECT_I::QS_TYPE_ROR_INTERSECT) ||
        (quick_type == QUICK_SELECT_I::QS_TYPE_ROR_UNION))
      tab_type= type == JT_ALL ? JT_INDEX_MERGE : JT_HASH_INDEX_MERGE;
    else
      tab_type= type == JT_ALL ? JT_RANGE : JT_HASH_RANGE;
  }
  eta->type= tab_type;

  /* Build "possible_keys" value */
  // psergey-todo: why does this use thd MEM_ROOT??? Doesn't this 
  // break ANALYZE ? thd->mem_root will be freed, and after that we will
  // attempt to print the query plan?
  append_possible_keys(thd->mem_root, eta->possible_keys, table, keys);
  // psergey-todo: ^ check for error return code 

  /* Build "key", "key_len", and "ref" */
  if (tab_type == JT_NEXT)
  {
    key_info= table->key_info+index;
    key_len= key_info->key_length;
  }
  else if (ref.key_parts)
  {
    key_info= get_keyinfo_by_key_no(ref.key);
    key_len= ref.key_length;
  }
  
  /*
    In STRAIGHT_JOIN queries, there can be join tabs with JT_CONST type
    that still have quick selects.
  */
  if (tab_select && tab_select->quick && tab_type != JT_CONST)
  {
    eta->quick_info= tab_select->quick->get_explain(thd->mem_root);
  }

  if (key_info) /* 'index' or 'ref' access */
  {
    eta->key.set(thd->mem_root, key_info, key_len);

    if (ref.key_parts && tab_type != JT_FT)
    {
      store_key **key_ref= ref.key_copy;
      for (uint kp= 0; kp < ref.key_parts; kp++)
      {
        if ((key_part_map(1) << kp) & ref.const_ref_part_map)
          eta->ref_list.append_str(thd->mem_root, "const");
        else
        {
          eta->ref_list.append_str(thd->mem_root, (*key_ref)->name());
          key_ref++;
        }
      }
    }
  }

  if (tab_type == JT_HASH_NEXT) /* full index scan + hash join */
  {
    eta->hash_next_key.set(thd->mem_root, 
                           & table->key_info[index], 
                           table->key_info[index].key_length);
    // psergey-todo: ^ is the above correct? are we necessarily joining on all
    // columns?
  }

  if (!key_info)
  {
    if (table_list && /* SJM bushes don't have table_list */
        table_list->schema_table &&
        table_list->schema_table->i_s_requested_object & OPTIMIZE_I_S_TABLE)
    {
      IS_table_read_plan *is_table_read_plan= table_list->is_table_read_plan;
      const char *tmp_buff;
      int f_idx;
      StringBuffer<64> key_name_buf;
      if (is_table_read_plan->trivial_show_command ||
          is_table_read_plan->has_db_lookup_value())
      {
        /* The "key" has the name of the column referring to the database */
        f_idx= table_list->schema_table->idx_field1;
        tmp_buff= table_list->schema_table->fields_info[f_idx].field_name;
        key_name_buf.append(tmp_buff, strlen(tmp_buff), cs);
      }          
      if (is_table_read_plan->trivial_show_command ||
          is_table_read_plan->has_table_lookup_value())
      {
        if (is_table_read_plan->trivial_show_command ||
            is_table_read_plan->has_db_lookup_value())
          key_name_buf.append(',');

        f_idx= table_list->schema_table->idx_field2;
        tmp_buff= table_list->schema_table->fields_info[f_idx].field_name;
        key_name_buf.append(tmp_buff, strlen(tmp_buff), cs);
      }

      if (key_name_buf.length())
        eta->key.set_pseudo_key(thd->mem_root, key_name_buf.c_ptr_safe());
    }
  }
  
  /* "rows" */
  if (table_list /* SJM bushes don't have table_list */ &&
      table_list->schema_table)
  {
    /* I_S tables have rows=extra=NULL */
    eta->rows_set= false;
    eta->filtered_set= false;
  }
  else
  {
    ha_rows examined_rows= get_examined_rows();

    eta->rows_set= true;
    eta->rows= examined_rows;

    /* "filtered"  */
    float f= 0.0; 
    if (examined_rows)
    {
      double pushdown_cond_selectivity= cond_selectivity;
      if (pushdown_cond_selectivity == 1.0)
        f= (float) (100.0 * records_read / examined_rows);
      else
        f= (float) (100.0 * pushdown_cond_selectivity);
    }
    set_if_smaller(f, 100.0);
    eta->filtered_set= true;
    eta->filtered= f;
  }

  /* Build "Extra" field and save it */
  key_read= table->file->keyread_enabled();
  if ((tab_type == JT_NEXT || tab_type == JT_CONST) &&
      table->covering_keys.is_set(index))
    key_read=1;
  if (quick_type == QUICK_SELECT_I::QS_TYPE_ROR_INTERSECT &&
      !((QUICK_ROR_INTERSECT_SELECT*)cur_quick)->need_to_fetch_row)
    key_read=1;
    
  if (info)
  {
    eta->push_extra(info);
  }
  else if (packed_info & TAB_INFO_HAVE_VALUE)
  {
    if (packed_info & TAB_INFO_USING_INDEX)
      eta->push_extra(ET_USING_INDEX);
    if (packed_info & TAB_INFO_USING_WHERE)
      eta->push_extra(ET_USING_WHERE);
    if (packed_info & TAB_INFO_FULL_SCAN_ON_NULL)
      eta->push_extra(ET_FULL_SCAN_ON_NULL_KEY);
  }
  else
  {
    uint keyno= MAX_KEY;
    if (ref.key_parts)
      keyno= ref.key;
    else if (tab_select && cur_quick)
      keyno = cur_quick->index;

    if (keyno != MAX_KEY && keyno == table->file->pushed_idx_cond_keyno &&
        table->file->pushed_idx_cond)
    {
      eta->push_extra(ET_USING_INDEX_CONDITION);
      eta->pushed_index_cond= table->file->pushed_idx_cond;
    }
    else if (cache_idx_cond)
    {
      eta->push_extra(ET_USING_INDEX_CONDITION_BKA);
      eta->pushed_index_cond= cache_idx_cond;
    }

    if (quick_type == QUICK_SELECT_I::QS_TYPE_ROR_UNION || 
        quick_type == QUICK_SELECT_I::QS_TYPE_ROR_INTERSECT ||
        quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_INTERSECT ||
        quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_MERGE)
    {
      eta->push_extra(ET_USING);
    }
    if (tab_select)
    {
      if (use_quick == 2)
      {
        eta->push_extra(ET_RANGE_CHECKED_FOR_EACH_RECORD);
        eta->range_checked_fer= new (thd->mem_root) Explain_range_checked_fer;
        if (eta->range_checked_fer)
          eta->range_checked_fer->
            append_possible_keys_stat(thd->mem_root, table, keys);
      }
      else if (tab_select->cond ||
               (cache_select && cache_select->cond))
      {
        const COND *pushed_cond= table->file->pushed_cond;

        if ((table->file->ha_table_flags() &
              HA_CAN_TABLE_CONDITION_PUSHDOWN) &&
            pushed_cond)
        {
          eta->push_extra(ET_USING_WHERE_WITH_PUSHED_CONDITION);
        }
        else
        {
          eta->where_cond= tab_select->cond;
          eta->cache_cond= cache_select? cache_select->cond : NULL;
          eta->push_extra(ET_USING_WHERE);
        }
      }
    }
    if (table_list /* SJM bushes don't have table_list */ &&
        table_list->schema_table &&
        table_list->schema_table->i_s_requested_object & OPTIMIZE_I_S_TABLE)
    {
      if (!table_list->table_open_method)
        eta->push_extra(ET_SKIP_OPEN_TABLE);
      else if (table_list->table_open_method == OPEN_FRM_ONLY)
        eta->push_extra(ET_OPEN_FRM_ONLY);
      else
        eta->push_extra(ET_OPEN_FULL_TABLE);
      /* psergey-note: the following has a bug.*/
      if (table_list->is_table_read_plan->trivial_show_command ||
          (table_list->is_table_read_plan->has_db_lookup_value() &&
           table_list->is_table_read_plan->has_table_lookup_value()))
        eta->push_extra(ET_SCANNED_0_DATABASES);
      else if (table_list->is_table_read_plan->has_db_lookup_value() ||
               table_list->is_table_read_plan->has_table_lookup_value())
        eta->push_extra(ET_SCANNED_1_DATABASE);
      else
        eta->push_extra(ET_SCANNED_ALL_DATABASES);
    }
    if (key_read)
    {
      if (quick_type == QUICK_SELECT_I::QS_TYPE_GROUP_MIN_MAX)
      {
        QUICK_GROUP_MIN_MAX_SELECT *qgs= 
          (QUICK_GROUP_MIN_MAX_SELECT *) tab_select->quick;
        eta->push_extra(ET_USING_INDEX_FOR_GROUP_BY);
        eta->loose_scan_is_scanning= qgs->loose_scan_is_scanning();
      }
      else
        eta->push_extra(ET_USING_INDEX);
    }
    if (table->reginfo.not_exists_optimize)
      eta->push_extra(ET_NOT_EXISTS);

    if (quick_type == QUICK_SELECT_I::QS_TYPE_RANGE)
    {
      explain_append_mrr_info((QUICK_RANGE_SELECT*)(tab_select->quick),
                              &eta->mrr_type);
      if (eta->mrr_type.length() > 0)
        eta->push_extra(ET_USING_MRR);
    }

    if (shortcut_for_distinct)
      eta->push_extra(ET_DISTINCT);

    if (loosescan_match_tab)
    {
      eta->push_extra(ET_LOOSESCAN);
    }

    if (first_weedout_table)
    {
      eta->start_dups_weedout= true;
      eta->push_extra(ET_START_TEMPORARY);
    }
    if (check_weed_out_table)
    {
      eta->push_extra(ET_END_TEMPORARY);
      eta->end_dups_weedout= true;
    }

    else if (do_firstmatch)
    {
      if (do_firstmatch == /*join->join_tab*/ first_top_tab - 1)
        eta->push_extra(ET_FIRST_MATCH);
      else
      {
        eta->push_extra(ET_FIRST_MATCH);
        TABLE *prev_table=do_firstmatch->table;
        if (prev_table->derived_select_number)
        {
          char namebuf[NAME_LEN];
          /* Derived table name generation */
          int len= my_snprintf(namebuf, sizeof(namebuf)-1,
                               "<derived%u>",
                               prev_table->derived_select_number);
          eta->firstmatch_table_name.append(namebuf, len);
        }
        else
          eta->firstmatch_table_name.append(prev_table->pos_in_table_list->alias);
      }
    }

    for (uint part= 0; part < ref.key_parts; part++)
    {
      if (ref.cond_guards[part])
      {
        eta->push_extra(ET_FULL_SCAN_ON_NULL_KEY);
        eta->full_scan_on_null_key= true;
        break;
      }
    }

    if (cache)
    {
      eta->push_extra(ET_USING_JOIN_BUFFER);
      cache->save_explain_data(&eta->bka_type);
    }
  }

  /* 
    In case this is a derived table, here we remember the number of 
    subselect that used to produce it.
  */
  if (!(table_list && table_list->is_with_table_recursive_reference()))
    eta->derived_select_number= table->derived_select_number;

  /* The same for non-merged semi-joins */
  eta->non_merged_sjm_number = get_non_merged_semijoin_select();
}