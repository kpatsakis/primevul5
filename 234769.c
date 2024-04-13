test_if_cheaper_ordering(const JOIN_TAB *tab, ORDER *order, TABLE *table,
                         key_map usable_keys,  int ref_key,
                         ha_rows select_limit_arg,
                         int *new_key, int *new_key_direction,
                         ha_rows *new_select_limit, uint *new_used_key_parts,
                         uint *saved_best_key_parts)
{
  DBUG_ENTER("test_if_cheaper_ordering");
  /*
    Check whether there is an index compatible with the given order
    usage of which is cheaper than usage of the ref_key index (ref_key>=0)
    or a table scan.
    It may be the case if ORDER/GROUP BY is used with LIMIT.
  */
  ha_rows best_select_limit= HA_POS_ERROR;
  JOIN *join= tab ? tab->join : NULL;
  uint nr;
  key_map keys;
  uint best_key_parts= 0;
  int best_key_direction= 0;
  ha_rows best_records= 0;
  double read_time;
  int best_key= -1;
  bool is_best_covering= FALSE;
  double fanout= 1;
  ha_rows table_records= table->stat_records();
  bool group= join && join->group && order == join->group_list;
  ha_rows refkey_rows_estimate= table->quick_condition_rows;
  const bool has_limit= (select_limit_arg != HA_POS_ERROR);

  /*
    If not used with LIMIT, only use keys if the whole query can be
    resolved with a key;  This is because filesort() is usually faster than
    retrieving all rows through an index.
  */
  if (select_limit_arg >= table_records)
  {
    keys= *table->file->keys_to_use_for_scanning();
    keys.merge(table->covering_keys);

    /*
      We are adding here also the index specified in FORCE INDEX clause, 
      if any.
      This is to allow users to use index in ORDER BY.
    */
    if (table->force_index) 
      keys.merge(group ? table->keys_in_use_for_group_by :
                         table->keys_in_use_for_order_by);
    keys.intersect(usable_keys);
  }
  else
    keys= usable_keys;

  if (join)
  {
    uint tablenr= (uint)(tab - join->join_tab);
    read_time= join->best_positions[tablenr].read_time;
    for (uint i= tablenr+1; i < join->table_count; i++)
      fanout*= join->best_positions[i].records_read; // fanout is always >= 1
  }
  else
    read_time= table->file->scan_time();
  
  /*
    TODO: add cost of sorting here.
  */
  read_time += COST_EPS;

  /*
    Calculate the selectivity of the ref_key for REF_ACCESS. For
    RANGE_ACCESS we use table->quick_condition_rows.
  */
  if (ref_key >= 0 && ref_key != MAX_KEY && tab->type == JT_REF)
  {
    /*
      If ref access uses keypart=const for all its key parts,
      and quick select uses the same # of key parts, then they are equivalent.
      Reuse #rows estimate from quick select as it is more precise.
    */
    if (tab->ref.const_ref_part_map ==
        make_prev_keypart_map(tab->ref.key_parts) &&
        table->quick_keys.is_set(ref_key) &&
        table->quick_key_parts[ref_key] == tab->ref.key_parts)
      refkey_rows_estimate= table->quick_rows[ref_key];
    else
    {
      const KEY *ref_keyinfo= table->key_info + ref_key;
      refkey_rows_estimate= ref_keyinfo->rec_per_key[tab->ref.key_parts - 1];
    }
    set_if_bigger(refkey_rows_estimate, 1);
  }

  for (nr=0; nr < table->s->keys ; nr++)
  {
    int direction;
    ha_rows select_limit= select_limit_arg;
    uint used_key_parts= 0;

    if (keys.is_set(nr) &&
        (direction= test_if_order_by_key(join, order, table, nr,
                                         &used_key_parts)))
    {
      /*
        At this point we are sure that ref_key is a non-ordering
        key (where "ordering key" is a key that will return rows
        in the order required by ORDER BY).
      */
      DBUG_ASSERT (ref_key != (int) nr);

      bool is_covering= (table->covering_keys.is_set(nr) ||
                         (table->file->index_flags(nr, 0, 1) &
                          HA_CLUSTERED_INDEX));
      /* 
        Don't use an index scan with ORDER BY without limit.
        For GROUP BY without limit always use index scan
        if there is a suitable index. 
        Why we hold to this asymmetry hardly can be explained
        rationally. It's easy to demonstrate that using
        temporary table + filesort could be cheaper for grouping
        queries too.
      */ 
      if (is_covering ||
          select_limit != HA_POS_ERROR || 
          (ref_key < 0 && (group || table->force_index)))
      { 
        double rec_per_key;
        double index_scan_time;
        KEY *keyinfo= table->key_info+nr;
        if (select_limit == HA_POS_ERROR)
          select_limit= table_records;
        if (group)
        {
          /* 
            Used_key_parts can be larger than keyinfo->user_defined_key_parts
            when using a secondary index clustered with a primary 
            key (e.g. as in Innodb). 
            See Bug #28591 for details.
          */  
          uint used_index_parts= keyinfo->user_defined_key_parts;
          uint used_pk_parts= 0;
          if (used_key_parts > used_index_parts)
            used_pk_parts= used_key_parts-used_index_parts;
          rec_per_key= used_key_parts ?
	               keyinfo->actual_rec_per_key(used_key_parts-1) : 1;
          /* Take into account the selectivity of the used pk prefix */
          if (used_pk_parts)
	  {
            KEY *pkinfo=tab->table->key_info+table->s->primary_key;
            /*
              If the values of of records per key for the prefixes
              of the primary key are considered unknown we assume
              they are equal to 1.
	    */
            if (used_key_parts == pkinfo->user_defined_key_parts ||
                pkinfo->rec_per_key[0] == 0)
              rec_per_key= 1;                 
            if (rec_per_key > 1)
	    {
              rec_per_key*= pkinfo->actual_rec_per_key(used_pk_parts-1);
              rec_per_key/= pkinfo->actual_rec_per_key(0);
              /* 
                The value of rec_per_key for the extended key has
                to be adjusted accordingly if some components of
                the secondary key are included in the primary key.
	      */
               for(uint i= 1; i < used_pk_parts; i++)
	      {
	        if (pkinfo->key_part[i].field->key_start.is_set(nr))
	        {
                  /* 
                    We presume here that for any index rec_per_key[i] != 0
                    if rec_per_key[0] != 0.
	          */
                  DBUG_ASSERT(pkinfo->actual_rec_per_key(i));
                  rec_per_key*= pkinfo->actual_rec_per_key(i-1);
                  rec_per_key/= pkinfo->actual_rec_per_key(i);
                }
	      }
            }    
          }
          set_if_bigger(rec_per_key, 1);
          /*
            With a grouping query each group containing on average
            rec_per_key records produces only one row that will
            be included into the result set.
          */  
          if (select_limit > table_records/rec_per_key)
            select_limit= table_records;
          else
            select_limit= (ha_rows) (select_limit*rec_per_key);
        } /* group */

        /* 
          If tab=tk is not the last joined table tn then to get first
          L records from the result set we can expect to retrieve
          only L/fanout(tk,tn) where fanout(tk,tn) says how many
          rows in the record set on average will match each row tk.
          Usually our estimates for fanouts are too pessimistic.
          So the estimate for L/fanout(tk,tn) will be too optimistic
          and as result we'll choose an index scan when using ref/range
          access + filesort will be cheaper.
        */
        select_limit= (ha_rows) (select_limit < fanout ?
                                 1 : select_limit/fanout);
        /*
          We assume that each of the tested indexes is not correlated
          with ref_key. Thus, to select first N records we have to scan
          N/selectivity(ref_key) index entries. 
          selectivity(ref_key) = #scanned_records/#table_records =
          refkey_rows_estimate/table_records.
          In any case we can't select more than #table_records.
          N/(refkey_rows_estimate/table_records) > table_records
          <=> N > refkey_rows_estimate.
         */
        if (select_limit > refkey_rows_estimate)
          select_limit= table_records;
        else
          select_limit= (ha_rows) (select_limit *
                                   (double) table_records /
                                    refkey_rows_estimate);
        rec_per_key= keyinfo->actual_rec_per_key(keyinfo->user_defined_key_parts-1);
        set_if_bigger(rec_per_key, 1);
        /*
          Here we take into account the fact that rows are
          accessed in sequences rec_per_key records in each.
          Rows in such a sequence are supposed to be ordered
          by rowid/primary key. When reading the data
          in a sequence we'll touch not more pages than the
          table file contains.
          TODO. Use the formula for a disk sweep sequential access
          to calculate the cost of accessing data rows for one 
          index entry.
        */
        index_scan_time= select_limit/rec_per_key *
                         MY_MIN(rec_per_key, table->file->scan_time());
        double range_scan_time;
        if (get_range_limit_read_cost(tab, table, nr, select_limit, 
                                       &range_scan_time))
        {
          if (range_scan_time < index_scan_time)
            index_scan_time= range_scan_time;
        }

        if ((ref_key < 0 && (group || table->force_index || is_covering)) ||
            index_scan_time < read_time)
        {
          ha_rows quick_records= table_records;
          ha_rows refkey_select_limit= (ref_key >= 0 &&
                                        !is_hash_join_key_no(ref_key) &&
                                        table->covering_keys.is_set(ref_key)) ?
                                        refkey_rows_estimate :
                                        HA_POS_ERROR;
          if ((is_best_covering && !is_covering) ||
              (is_covering && refkey_select_limit < select_limit))
            continue;
          if (table->quick_keys.is_set(nr))
            quick_records= table->quick_rows[nr];
          if (best_key < 0 ||
              (select_limit <= MY_MIN(quick_records,best_records) ?
               keyinfo->user_defined_key_parts < best_key_parts :
               quick_records < best_records) ||
              (!is_best_covering && is_covering))
          {
            best_key= nr;
            best_key_parts= keyinfo->user_defined_key_parts;
            if (saved_best_key_parts)
              *saved_best_key_parts= used_key_parts;
            best_records= quick_records;
            is_best_covering= is_covering;
            best_key_direction= direction; 
            best_select_limit= select_limit;
          }
        }   
      }      
    }
  }

  if (best_key < 0 || best_key == ref_key)
    DBUG_RETURN(FALSE);
  
  *new_key= best_key;
  *new_key_direction= best_key_direction;
  *new_select_limit= has_limit ? best_select_limit : table_records;
  if (new_used_key_parts != NULL)
    *new_used_key_parts= best_key_parts;

  DBUG_RETURN(TRUE);
}