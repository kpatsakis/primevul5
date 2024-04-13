test_if_skip_sort_order(JOIN_TAB *tab,ORDER *order,ha_rows select_limit,
			bool no_changes, const key_map *map)
{
  int ref_key;
  uint UNINIT_VAR(ref_key_parts);
  int order_direction= 0;
  uint used_key_parts= 0;
  TABLE *table=tab->table;
  SQL_SELECT *select=tab->select;
  key_map usable_keys;
  QUICK_SELECT_I *save_quick= select ? select->quick : 0;
  Item *orig_cond= 0;
  bool orig_cond_saved= false;
  int best_key= -1;
  bool changed_key= false;
  DBUG_ENTER("test_if_skip_sort_order");

  /* Check that we are always called with first non-const table */
  DBUG_ASSERT(tab == tab->join->join_tab + tab->join->const_tables);

  /*
    Keys disabled by ALTER TABLE ... DISABLE KEYS should have already
    been taken into account.
  */
  usable_keys= *map;
  
  /* Find indexes that cover all ORDER/GROUP BY fields */
  for (ORDER *tmp_order=order; tmp_order ; tmp_order=tmp_order->next)
  {
    Item *item= (*tmp_order->item)->real_item();
    if (item->type() != Item::FIELD_ITEM)
    {
      usable_keys.clear_all();
      DBUG_RETURN(0);
    }

    /*
      Take multiple-equalities into account. Suppose we have
        ORDER BY col1, col10
      and there are
         multiple-equal(col1, col2, col3),
         multiple-equal(col10, col11).

      Then, 
      - when item=col1, we find the set of indexes that cover one of {col1,
        col2, col3}
      - when item=col10, we find the set of indexes that cover one of {col10,
        col11}

      And we compute an intersection of these sets to find set of indexes that
      cover all ORDER BY components.
    */
    key_map col_keys;
    compute_part_of_sort_key_for_equals(tab->join, table, (Item_field*)item,
                                        &col_keys);
    usable_keys.intersect(col_keys);
    if (usable_keys.is_clear_all())
      goto use_filesort;                        // No usable keys
  }

  ref_key= -1;
  /* Test if constant range in WHERE */
  if (tab->ref.key >= 0 && tab->ref.key_parts)
  {
    ref_key=	   tab->ref.key;
    ref_key_parts= tab->ref.key_parts;
    /* 
      todo: why does JT_REF_OR_NULL mean filesort? We could find another index
      that satisfies the ordering. I would just set ref_key=MAX_KEY here...
    */
    if (tab->type == JT_REF_OR_NULL || tab->type == JT_FT ||
        tab->ref.uses_splitting)
      goto use_filesort;
  }
  else if (select && select->quick)		// Range found by opt_range
  {
    int quick_type= select->quick->get_type();
    /* 
      assume results are not ordered when index merge is used 
      TODO: sergeyp: Results of all index merge selects actually are ordered 
      by clustered PK values.
    */
  
    if (quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_MERGE ||
        quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_INTERSECT ||
        quick_type == QUICK_SELECT_I::QS_TYPE_ROR_UNION || 
        quick_type == QUICK_SELECT_I::QS_TYPE_ROR_INTERSECT)
    {
      /*
        we set ref_key=MAX_KEY instead of -1, because test_if_cheaper ordering
        assumes that "ref_key==-1" means doing full index scan. 
        (This is not very straightforward and we got into this situation for 
         historical reasons. Should be fixed at some point).
      */
      ref_key= MAX_KEY;
    }
    else
    {
      ref_key= select->quick->index;
      ref_key_parts= select->quick->used_key_parts;
    }
  }

  if (ref_key >= 0 && ref_key != MAX_KEY)
  {
    /* Current access method uses index ref_key with ref_key_parts parts */
    if (!usable_keys.is_set(ref_key))
    {
      /* However, ref_key doesn't match the needed ordering */
      uint new_ref_key;

      /*
	If using index only read, only consider other possible index only
	keys
      */
      if (table->covering_keys.is_set(ref_key))
	usable_keys.intersect(table->covering_keys);
      if (tab->pre_idx_push_select_cond)
      {
        orig_cond= tab->set_cond(tab->pre_idx_push_select_cond);
        orig_cond_saved= true;
      }

      if ((new_ref_key= test_if_subkey(order, table, ref_key, ref_key_parts,
				       &usable_keys)) < MAX_KEY)
      {
        /*
          Index new_ref_key 
          - produces the required ordering, 
          - also has the same columns as ref_key for #ref_key_parts (this
            means we will read the same number of rows as with ref_key).
        */

        /*
          If new_ref_key allows to construct a quick select which uses more key
          parts than ref(new_ref_key) would, do that.

          Otherwise, construct a ref access (todo: it's not clear what is the
          win in using ref access when we could use quick select also?)
        */
        if ((table->quick_keys.is_set(new_ref_key) && 
             table->quick_key_parts[new_ref_key] > ref_key_parts) ||
             !(tab->ref.key >= 0))
	{
          /*
            The range optimizer constructed QUICK_RANGE for ref_key, and
            we want to use instead new_ref_key as the index. We can't
            just change the index of the quick select, because this may
            result in an inconsistent QUICK_SELECT object. Below we
            create a new QUICK_SELECT from scratch so that all its
            parameters are set correctly by the range optimizer.
           */
          key_map new_ref_key_map;
          COND *save_cond;
          bool res;
          new_ref_key_map.clear_all();  // Force the creation of quick select
          new_ref_key_map.set_bit(new_ref_key); // only for new_ref_key.

          /* Reset quick;  This will be restored in 'use_filesort' if needed */
          select->quick= 0;
          save_cond= select->cond;
          if (select->pre_idx_push_select_cond)
            select->cond= select->pre_idx_push_select_cond;
          res= select->test_quick_select(tab->join->thd, new_ref_key_map, 0,
                                         (tab->join->select_options &
                                          OPTION_FOUND_ROWS) ?
                                         HA_POS_ERROR :
                                         tab->join->unit->select_limit_cnt,TRUE,
                                         TRUE, FALSE) <= 0;
          if (res)
          {
            select->cond= save_cond;
            goto use_filesort;
          }
          DBUG_ASSERT(tab->select->quick);
          tab->type= JT_ALL;
          tab->ref.key= -1;
          tab->ref.key_parts= 0;
          tab->use_quick= 1;
          best_key= new_ref_key;
          /*
            We don't restore select->cond as we want to use the
            original condition as index condition pushdown is not
            active for the new index.
            todo: why not perform index condition pushdown for the new index?
          */
	}
        else
	{
          /*
            We'll use ref access method on key new_ref_key. In general case 
            the index search tuple for new_ref_key will be different (e.g.
            when one index is defined as (part1, part2, ...) and another as
            (part1, part2(N), ...) and the WHERE clause contains 
            "part1 = const1 AND part2=const2". 
            So we build tab->ref from scratch here.
          */
          KEYUSE *keyuse= tab->keyuse;
          while (keyuse->key != new_ref_key && keyuse->table == tab->table)
            keyuse++;
          if (create_ref_for_key(tab->join, tab, keyuse, FALSE,
                                 (tab->join->const_table_map |
                                  OUTER_REF_TABLE_BIT)))
            goto use_filesort;

          pick_table_access_method(tab);
	}

        ref_key= new_ref_key;
        changed_key= true;
     }
    }
    /* Check if we get the rows in requested sorted order by using the key */
    if (usable_keys.is_set(ref_key) &&
        (order_direction= test_if_order_by_key(tab->join, order,table,ref_key,
					       &used_key_parts)))
      goto check_reverse_order;
  }
  {
    uint UNINIT_VAR(best_key_parts);
    uint saved_best_key_parts= 0;
    int best_key_direction= 0;
    JOIN *join= tab->join;
    ha_rows table_records= table->stat_records();

    test_if_cheaper_ordering(tab, order, table, usable_keys,
                             ref_key, select_limit,
                             &best_key, &best_key_direction,
                             &select_limit, &best_key_parts,
                             &saved_best_key_parts);

    /*
      filesort() and join cache are usually faster than reading in 
      index order and not using join cache, except in case that chosen
      index is clustered key.
    */
    if (best_key < 0 ||
        ((select_limit >= table_records) &&
         (tab->type == JT_ALL &&
         tab->join->table_count > tab->join->const_tables + 1) &&
         !(table->file->index_flags(best_key, 0, 1) & HA_CLUSTERED_INDEX)))
      goto use_filesort;

    if (select && // psergey:  why doesn't this use a quick?
        table->quick_keys.is_set(best_key) && best_key != ref_key)
    {
      key_map tmp_map;
      tmp_map.clear_all();       // Force the creation of quick select
      tmp_map.set_bit(best_key); // only best_key.
      select->quick= 0;

      bool cond_saved= false;
      Item *saved_cond;

      /*
        Index Condition Pushdown may have removed parts of the condition for
        this table. Temporarily put them back because we want the whole
        condition for the range analysis.
      */
      if (select->pre_idx_push_select_cond)
      {
        saved_cond= select->cond;
        select->cond= select->pre_idx_push_select_cond;
        cond_saved= true;
      }

      select->test_quick_select(join->thd, tmp_map, 0,
                                join->select_options & OPTION_FOUND_ROWS ?
                                HA_POS_ERROR :
                                join->unit->select_limit_cnt,
                                TRUE, FALSE, FALSE);

      if (cond_saved)
        select->cond= saved_cond;
    }
    order_direction= best_key_direction;
    /*
      saved_best_key_parts is actual number of used keyparts found by the
      test_if_order_by_key function. It could differ from keyinfo->user_defined_key_parts,
      thus we have to restore it in case of desc order as it affects
      QUICK_SELECT_DESC behaviour.
    */
    used_key_parts= (order_direction == -1) ?
      saved_best_key_parts :  best_key_parts;
    changed_key= true;
  }

check_reverse_order:                  
  DBUG_ASSERT(order_direction != 0);

  if (order_direction == -1)		// If ORDER BY ... DESC
  {
    int quick_type;
    if (select && select->quick)
    {
      /*
	Don't reverse the sort order, if it's already done.
        (In some cases test_if_order_by_key() can be called multiple times
      */
      if (select->quick->reverse_sorted())
        goto skipped_filesort;

      quick_type= select->quick->get_type();
      if (quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_MERGE ||
          quick_type == QUICK_SELECT_I::QS_TYPE_INDEX_INTERSECT ||
          quick_type == QUICK_SELECT_I::QS_TYPE_ROR_INTERSECT ||
          quick_type == QUICK_SELECT_I::QS_TYPE_ROR_UNION ||
          quick_type == QUICK_SELECT_I::QS_TYPE_GROUP_MIN_MAX)
      {
        tab->limit= 0;
        goto use_filesort;               // Use filesort
      }
    }
  }

  /*
    Update query plan with access pattern for doing ordered access
    according to what we have decided above.
  */
  if (!no_changes) // We are allowed to update QEP
  {
    if (best_key >= 0)
    {
      bool quick_created= 
        (select && select->quick && select->quick!=save_quick);

      /* 
         If ref_key used index tree reading only ('Using index' in EXPLAIN),
         and best_key doesn't, then revert the decision.
      */
      if (table->covering_keys.is_set(best_key))
        table->file->ha_start_keyread(best_key);
      else
        table->file->ha_end_keyread();

      if (!quick_created)
      {
        if (select)                  // Throw any existing quick select
          select->quick= 0;          // Cleanup either reset to save_quick,
                                     // or 'delete save_quick'
        tab->index= best_key;
        tab->read_first_record= order_direction > 0 ?
                                join_read_first:join_read_last;
        tab->type=JT_NEXT;           // Read with index_first(), index_next()

        if (tab->pre_idx_push_select_cond)
        {
          tab->set_cond(tab->pre_idx_push_select_cond);
          /*
            orig_cond is a part of pre_idx_push_cond,
            no need to restore it.
          */
          orig_cond= 0;
          orig_cond_saved= false;
        }

        table->file->ha_index_or_rnd_end();
        if (tab->join->select_options & SELECT_DESCRIBE)
        {
          tab->ref.key= -1;
          tab->ref.key_parts= 0;
          if (select_limit < table->stat_records())
            tab->limit= select_limit;
          table->file->ha_end_keyread();
        }
      }
      else if (tab->type != JT_ALL || tab->select->quick)
      {
        /*
          We're about to use a quick access to the table.
          We need to change the access method so as the quick access
          method is actually used.
        */
        DBUG_ASSERT(tab->select->quick);
        tab->type=JT_ALL;
        tab->use_quick=1;
        tab->ref.key= -1;
        tab->ref.key_parts=0;		// Don't use ref key.
        tab->read_first_record= join_init_read_record;
        if (tab->is_using_loose_index_scan())
          tab->join->tmp_table_param.precomputed_group_by= TRUE;

        /*
          Restore the original condition as changes done by pushdown
          condition are not relevant anymore
        */
        if (tab->select && tab->select->pre_idx_push_select_cond)
	{
          tab->set_cond(tab->select->pre_idx_push_select_cond);
           tab->table->file->cancel_pushed_idx_cond();
        }
        /*
          TODO: update the number of records in join->best_positions[tablenr]
        */
      }
    } // best_key >= 0

    if (order_direction == -1)		// If ORDER BY ... DESC
    {
      if (select && select->quick)
      {
        /* ORDER BY range_key DESC */
        QUICK_SELECT_I *tmp= select->quick->make_reverse(used_key_parts);
        if (!tmp)
        {
          tab->limit= 0;
          goto use_filesort;           // Reverse sort failed -> filesort
        }
        /*
          Cancel Pushed Index Condition, as it doesn't work for reverse scans.
        */
        if (tab->select && tab->select->pre_idx_push_select_cond)
	{
          tab->set_cond(tab->select->pre_idx_push_select_cond);
           tab->table->file->cancel_pushed_idx_cond();
        }
        if (select->quick == save_quick)
          save_quick= 0;                // make_reverse() consumed it
        select->set_quick(tmp);
        /* Cancel "Range checked for each record" */
        if (tab->use_quick == 2)
        {
          tab->use_quick= 1;
          tab->read_first_record= join_init_read_record;
        }
      }
      else if (tab->type != JT_NEXT && tab->type != JT_REF_OR_NULL &&
               tab->ref.key >= 0 && tab->ref.key_parts <= used_key_parts)
      {
        /*
          SELECT * FROM t1 WHERE a=1 ORDER BY a DESC,b DESC

          Use a traversal function that starts by reading the last row
          with key part (A) and then traverse the index backwards.
        */
        tab->read_first_record= join_read_last_key;
        tab->read_record.read_record_func= join_read_prev_same;
        /* Cancel "Range checked for each record" */
        if (tab->use_quick == 2)
        {
          tab->use_quick= 1;
          tab->read_first_record= join_init_read_record;
        }
        /*
          Cancel Pushed Index Condition, as it doesn't work for reverse scans.
        */
        if (tab->select && tab->select->pre_idx_push_select_cond)
	{
          tab->set_cond(tab->select->pre_idx_push_select_cond);
           tab->table->file->cancel_pushed_idx_cond();
        }
      }
    }
    else if (select && select->quick)
      select->quick->need_sorted_output();

    tab->read_record.unlock_row= (tab->type == JT_EQ_REF) ?
                                 join_read_key_unlock_row : rr_unlock_row;

  } // QEP has been modified

  /*
    Cleanup:
    We may have both a 'select->quick' and 'save_quick' (original)
    at this point. Delete the one that we wan't use.
  */

skipped_filesort:
  // Keep current (ordered) select->quick 
  if (select && save_quick != select->quick)
  {
    delete save_quick;
    save_quick= NULL;
  }
  if (orig_cond_saved && !changed_key)
    tab->set_cond(orig_cond);
  if (!no_changes && changed_key && table->file->pushed_idx_cond)
    table->file->cancel_pushed_idx_cond();

  DBUG_RETURN(1);

use_filesort:
  // Restore original save_quick
  if (select && select->quick != save_quick)
  {
    delete select->quick;
    select->quick= save_quick;
  }
  if (orig_cond_saved)
    tab->set_cond(orig_cond);

  DBUG_RETURN(0);
}