best_access_path(JOIN      *join,
                 JOIN_TAB  *s,
                 table_map remaining_tables,
                 const POSITION *join_positions,
                 uint      idx,
                 bool      disable_jbuf,
                 double    record_count,
                 POSITION *pos,
                 POSITION *loose_scan_pos)
{
  THD *thd= join->thd;
  uint use_cond_selectivity= thd->variables.optimizer_use_condition_selectivity;
  KEYUSE *best_key=         0;
  uint best_max_key_part=   0;
  my_bool found_constraint= 0;
  double best=              DBL_MAX;
  double best_time=         DBL_MAX;
  double records=           DBL_MAX;
  table_map best_ref_depends_map= 0;
  double tmp;
  ha_rows rec;
  bool best_uses_jbuf= FALSE;
  MY_BITMAP *eq_join_set= &s->table->eq_join_set;
  KEYUSE *hj_start_key= 0;
  SplM_plan_info *spl_plan= 0;

  disable_jbuf= disable_jbuf || idx == join->const_tables;  

  Loose_scan_opt loose_scan_opt;
  DBUG_ENTER("best_access_path");
  
  bitmap_clear_all(eq_join_set);

  loose_scan_opt.init(join, s, remaining_tables);

  if (s->table->is_splittable())
    spl_plan= s->choose_best_splitting(record_count, remaining_tables);

  if (s->keyuse)
  {                                            /* Use key if possible */
    KEYUSE *keyuse;
    KEYUSE *start_key=0;
    TABLE *table= s->table;
    double best_records= DBL_MAX;
    uint max_key_part=0;

    /* Test how we can use keys */
    rec= s->records/MATCHING_ROWS_IN_OTHER_TABLE;  // Assumed records/key
    for (keyuse=s->keyuse ; keyuse->table == table ;)
    {
      KEY *keyinfo;
      ulong key_flags;
      uint key_parts;
      key_part_map found_part= 0;
      key_part_map notnull_part=0; // key parts which won't have NULL in lookup tuple.
      table_map found_ref= 0;
      uint key= keyuse->key;
      bool ft_key=  (keyuse->keypart == FT_KEYPART);
      /* Bitmap of keyparts where the ref access is over 'keypart=const': */
      key_part_map const_part= 0;
      /* The or-null keypart in ref-or-null access: */
      key_part_map ref_or_null_part= 0;
      if (is_hash_join_key_no(key))
      {
        /* 
          Hash join as any join employing join buffer can be used to join
          only those tables that are joined after the first non const table
	*/  
        if (!(remaining_tables & keyuse->used_tables) &&
            idx > join->const_tables)
        {
          if (!hj_start_key)
            hj_start_key= keyuse;
          bitmap_set_bit(eq_join_set, keyuse->keypart);
        }
        keyuse++;
        continue;
      }

      keyinfo= table->key_info+key;
      key_parts= table->actual_n_key_parts(keyinfo);
      key_flags= table->actual_key_flags(keyinfo);

      /* Calculate how many key segments of the current key we can use */
      start_key= keyuse;

      loose_scan_opt.next_ref_key();
      DBUG_PRINT("info", ("Considering ref access on key %s",
                          keyuse->table->key_info[keyuse->key].name.str));

      do /* For each keypart */
      {
        uint keypart= keyuse->keypart;
        table_map best_part_found_ref= 0;
        double best_prev_record_reads= DBL_MAX;
        
        do /* For each way to access the keypart */
        {
          /*
            if 1. expression doesn't refer to forward tables
               2. we won't get two ref-or-null's
          */
          if (!(remaining_tables & keyuse->used_tables) &&
              (!keyuse->validity_ref || *keyuse->validity_ref) &&
              s->access_from_tables_is_allowed(keyuse->used_tables,
                                               join->sjm_lookup_tables) &&
              !(ref_or_null_part && (keyuse->optimize &
                                     KEY_OPTIMIZE_REF_OR_NULL)))
          {
            found_part|= keyuse->keypart_map;
            if (!(keyuse->used_tables & ~join->const_table_map))
              const_part|= keyuse->keypart_map;

            if (!keyuse->val->maybe_null || keyuse->null_rejecting)
              notnull_part|=keyuse->keypart_map;

            double tmp2= prev_record_reads(join_positions, idx,
                                           (found_ref | keyuse->used_tables));
            if (tmp2 < best_prev_record_reads)
            {
              best_part_found_ref= keyuse->used_tables & ~join->const_table_map;
              best_prev_record_reads= tmp2;
            }
            if (rec > keyuse->ref_table_rows)
              rec= keyuse->ref_table_rows;
	    /*
	      If there is one 'key_column IS NULL' expression, we can
	      use this ref_or_null optimisation of this field
	    */
            if (keyuse->optimize & KEY_OPTIMIZE_REF_OR_NULL)
              ref_or_null_part |= keyuse->keypart_map;
          }
          loose_scan_opt.add_keyuse(remaining_tables, keyuse);
          keyuse++;
        } while (keyuse->table == table && keyuse->key == key &&
                 keyuse->keypart == keypart);
	found_ref|= best_part_found_ref;
      } while (keyuse->table == table && keyuse->key == key);

      /*
        Assume that that each key matches a proportional part of table.
      */
      if (!found_part && !ft_key && !loose_scan_opt.have_a_case())
        continue;                               // Nothing usable found

      if (rec < MATCHING_ROWS_IN_OTHER_TABLE)
        rec= MATCHING_ROWS_IN_OTHER_TABLE;      // Fix for small tables

      /*
        ft-keys require special treatment
      */
      if (ft_key)
      {
        /*
          Really, there should be records=0.0 (yes!)
          but 1.0 would be probably safer
        */
        tmp= prev_record_reads(join_positions, idx, found_ref);
        records= 1.0;
      }
      else
      {
        found_constraint= MY_TEST(found_part);
        loose_scan_opt.check_ref_access_part1(s, key, start_key, found_part);

        /* Check if we found full key */
        const key_part_map all_key_parts= PREV_BITS(uint, key_parts);
        if (found_part == all_key_parts && !ref_or_null_part)
        {                                         /* use eq key */
          max_key_part= (uint) ~0;
          /*
            If the index is a unique index (1), and
            - all its columns are not null (2), or
            - equalities we are using reject NULLs (3)
            then the estimate is rows=1.
          */
          if ((key_flags & (HA_NOSAME | HA_EXT_NOSAME)) &&   // (1)
              (!(key_flags & HA_NULL_PART_KEY) ||            //  (2)
               all_key_parts == notnull_part))               //  (3)
          {
            tmp = prev_record_reads(join_positions, idx, found_ref);
            records=1.0;
          }
          else
          {
            if (!found_ref)
            {                                     /* We found a const key */
              /*
                ReuseRangeEstimateForRef-1:
                We get here if we've found a ref(const) (c_i are constants):
                  "(keypart1=c1) AND ... AND (keypartN=cN)"   [ref_const_cond]
                
                If range optimizer was able to construct a "range" 
                access on this index, then its condition "quick_cond" was
                eqivalent to ref_const_cond (*), and we can re-use E(#rows)
                from the range optimizer.
                
                Proof of (*): By properties of range and ref optimizers 
                quick_cond will be equal or tighther than ref_const_cond. 
                ref_const_cond already covers "smallest" possible interval - 
                a singlepoint interval over all keyparts. Therefore, 
                quick_cond is equivalent to ref_const_cond (if it was an 
                empty interval we wouldn't have got here).
              */
              if (table->quick_keys.is_set(key))
                records= (double) table->quick_rows[key];
              else
              {
                /* quick_range couldn't use key! */
                records= (double) s->records/rec;
              }
            }
            else
            {
              if (!(records= keyinfo->actual_rec_per_key(key_parts-1)))
              {                                   /* Prefer longer keys */
                records=
                  ((double) s->records / (double) rec *
                   (1.0 +
                    ((double) (table->s->max_key_length-keyinfo->key_length) /
                     (double) table->s->max_key_length)));
                if (records < 2.0)
                  records=2.0;               /* Can't be as good as a unique */
              }
              /*
                ReuseRangeEstimateForRef-2:  We get here if we could not reuse
                E(#rows) from range optimizer. Make another try:
                
                If range optimizer produced E(#rows) for a prefix of the ref
                access we're considering, and that E(#rows) is lower then our
                current estimate, make an adjustment. The criteria of when we
                can make an adjustment is a special case of the criteria used
                in ReuseRangeEstimateForRef-3.
              */
              if (table->quick_keys.is_set(key) &&
                  (const_part &
                    (((key_part_map)1 << table->quick_key_parts[key])-1)) ==
                  (((key_part_map)1 << table->quick_key_parts[key])-1) &&
                  table->quick_n_ranges[key] == 1 &&
                  records > (double) table->quick_rows[key])
              {
                records= (double) table->quick_rows[key];
              }
            }
            /* Limit the number of matched rows */
            tmp= records;
            set_if_smaller(tmp, (double) thd->variables.max_seeks_for_key);
            if (table->covering_keys.is_set(key))
              tmp= table->file->keyread_time(key, 1, (ha_rows) tmp);
            else
              tmp= table->file->read_time(key, 1,
                                          (ha_rows) MY_MIN(tmp,s->worst_seeks));
            tmp= COST_MULT(tmp, record_count);
          }
        }
        else
        {
          /*
            Use as much key-parts as possible and a uniq key is better
            than a not unique key
            Set tmp to (previous record count) * (records / combination)
          */
          if ((found_part & 1) &&
              (!(table->file->index_flags(key, 0, 0) & HA_ONLY_WHOLE_INDEX) ||
               found_part == PREV_BITS(uint,keyinfo->user_defined_key_parts)))
          {
            max_key_part= max_part_bit(found_part);
            /*
              ReuseRangeEstimateForRef-3:
              We're now considering a ref[or_null] access via
              (t.keypart1=e1 AND ... AND t.keypartK=eK) [ OR  
              (same-as-above but with one cond replaced 
               with "t.keypart_i IS NULL")]  (**)
              
              Try re-using E(#rows) from "range" optimizer:
              We can do so if "range" optimizer used the same intervals as
              in (**). The intervals used by range optimizer may be not 
              available at this point (as "range" access might have choosen to
              create quick select over another index), so we can't compare
              them to (**). We'll make indirect judgements instead.
              The sufficient conditions for re-use are:
              (C1) All e_i in (**) are constants, i.e. found_ref==FALSE. (if
                   this is not satisfied we have no way to know which ranges
                   will be actually scanned by 'ref' until we execute the 
                   join)
              (C2) max #key parts in 'range' access == K == max_key_part (this
                   is apparently a necessary requirement)

              We also have a property that "range optimizer produces equal or 
              tighter set of scan intervals than ref(const) optimizer". Each
              of the intervals in (**) are "tightest possible" intervals when 
              one limits itself to using keyparts 1..K (which we do in #2).              
              From here it follows that range access used either one, or
              both of the (I1) and (I2) intervals:
              
               (t.keypart1=c1 AND ... AND t.keypartK=eK)  (I1) 
               (same-as-above but with one cond replaced  
                with "t.keypart_i IS NULL")               (I2)

              The remaining part is to exclude the situation where range
              optimizer used one interval while we're considering
              ref-or-null and looking for estimate for two intervals. This
              is done by last limitation:

              (C3) "range optimizer used (have ref_or_null?2:1) intervals"
            */
            if (table->quick_keys.is_set(key) && !found_ref &&          //(C1)
                table->quick_key_parts[key] == max_key_part &&          //(C2)
                table->quick_n_ranges[key] == 1 + MY_TEST(ref_or_null_part)) //(C3)
            {
              tmp= records= (double) table->quick_rows[key];
            }
            else
            {
              /* Check if we have statistic about the distribution */
              if ((records= keyinfo->actual_rec_per_key(max_key_part-1)))
              {
                /* 
                  Fix for the case where the index statistics is too
                  optimistic: If 
                  (1) We're considering ref(const) and there is quick select
                      on the same index, 
                  (2) and that quick select uses more keyparts (i.e. it will
                      scan equal/smaller interval then this ref(const))
                  (3) and E(#rows) for quick select is higher then our
                      estimate,
                  Then 
                    We'll use E(#rows) from quick select.

                  Q: Why do we choose to use 'ref'? Won't quick select be
                  cheaper in some cases ?
                  TODO: figure this out and adjust the plan choice if needed.
                */
                if (!found_ref && table->quick_keys.is_set(key) &&    // (1)
                    table->quick_key_parts[key] > max_key_part &&     // (2)
                    records < (double)table->quick_rows[key])         // (3)
                  records= (double)table->quick_rows[key];

                tmp= records;
              }
              else
              {
                /*
                  Assume that the first key part matches 1% of the file
                  and that the whole key matches 10 (duplicates) or 1
                  (unique) records.
                  Assume also that more key matches proportionally more
                  records
                  This gives the formula:
                  records = (x * (b-a) + a*c-b)/(c-1)

                  b = records matched by whole key
                  a = records matched by first key part (1% of all records?)
                  c = number of key parts in key
                  x = used key parts (1 <= x <= c)
                */
                double rec_per_key;
                if (!(rec_per_key=(double)
                      keyinfo->rec_per_key[keyinfo->user_defined_key_parts-1]))
                  rec_per_key=(double) s->records/rec+1;

                if (!s->records)
                  tmp = 0;
                else if (rec_per_key/(double) s->records >= 0.01)
                  tmp = rec_per_key;
                else
                {
                  double a=s->records*0.01;
                  if (keyinfo->user_defined_key_parts > 1)
                    tmp= (max_key_part * (rec_per_key - a) +
                          a*keyinfo->user_defined_key_parts - rec_per_key)/
                         (keyinfo->user_defined_key_parts-1);
                  else
                    tmp= a;
                  set_if_bigger(tmp,1.0);
                }
                records = (ulong) tmp;
              }

              if (ref_or_null_part)
              {
                /* We need to do two key searches to find key */
                tmp *= 2.0;
                records *= 2.0;
              }

              /*
                ReuseRangeEstimateForRef-4:  We get here if we could not reuse
                E(#rows) from range optimizer. Make another try:
                
                If range optimizer produced E(#rows) for a prefix of the ref 
                access we're considering, and that E(#rows) is lower then our
                current estimate, make the adjustment.

                The decision whether we can re-use the estimate from the range
                optimizer is the same as in ReuseRangeEstimateForRef-3,
                applied to first table->quick_key_parts[key] key parts.
              */
              if (table->quick_keys.is_set(key) &&
                  table->quick_key_parts[key] <= max_key_part &&
                  const_part &
                    ((key_part_map)1 << table->quick_key_parts[key]) &&
                  table->quick_n_ranges[key] == 1 + MY_TEST(ref_or_null_part &
                                                            const_part) &&
                  records > (double) table->quick_rows[key])
              {
                tmp= records= (double) table->quick_rows[key];
              }
            }

            /* Limit the number of matched rows */
            set_if_smaller(tmp, (double) thd->variables.max_seeks_for_key);
            if (table->covering_keys.is_set(key))
              tmp= table->file->keyread_time(key, 1, (ha_rows) tmp);
            else
              tmp= table->file->read_time(key, 1,
                                          (ha_rows) MY_MIN(tmp,s->worst_seeks));
            tmp= COST_MULT(tmp, record_count);
          }
          else
            tmp= best_time;                     // Do nothing
        }

        tmp= COST_ADD(tmp, s->startup_cost);
        loose_scan_opt.check_ref_access_part2(key, start_key, records, tmp,
                                              found_ref);
      } /* not ft_key */

      if (tmp + 0.0001 < best_time - records/(double) TIME_FOR_COMPARE)
      {
        best_time= COST_ADD(tmp, records/(double) TIME_FOR_COMPARE);
        best= tmp;
        best_records= records;
        best_key= start_key;
        best_max_key_part= max_key_part;
        best_ref_depends_map= found_ref;
      }
    } /* for each key */
    records= best_records;
  }

  /* 
    If there is no key to access the table, but there is an equi-join
    predicate connecting the table with the privious tables then we
    consider the possibility of using hash join.
    We need also to check that:
    (1) s is inner table of semi-join -> join cache is allowed for semijoins
    (2) s is inner table of outer join -> join cache is allowed for outer joins
  */  
  if (idx > join->const_tables && best_key == 0 &&
      (join->allowed_join_cache_types & JOIN_CACHE_HASHED_BIT) &&
      join->max_allowed_join_cache_level > 2 &&
     !bitmap_is_clear_all(eq_join_set) &&  !disable_jbuf &&
      (!s->emb_sj_nest ||                     
       join->allowed_semijoin_with_cache) &&    // (1)
      (!(s->table->map & join->outer_join) ||
       join->allowed_outer_join_with_cache))    // (2)
  {
    double join_sel= 0.1;
    /* Estimate the cost of  the hash join access to the table */
    double rnd_records= matching_candidates_in_table(s, found_constraint,
                                                     use_cond_selectivity);

    tmp= s->quick ? s->quick->read_time : s->scan_time();
    double cmp_time= (s->records - rnd_records)/(double) TIME_FOR_COMPARE;
    tmp= COST_ADD(tmp, cmp_time);

    /* We read the table as many times as join buffer becomes full. */

    double refills= (1.0 + floor((double) cache_record_length(join,idx) *
                           record_count /
			   (double) thd->variables.join_buff_size));
    tmp= COST_MULT(tmp, refills);
    best_time= COST_ADD(tmp,
                        COST_MULT((record_count*join_sel) / TIME_FOR_COMPARE,
                                  rnd_records));
    best= tmp;
    records= rnd_records;
    best_key= hj_start_key;
    best_ref_depends_map= 0;
    best_uses_jbuf= TRUE;
   }

  /*
    Don't test table scan if it can't be better.
    Prefer key lookup if we would use the same key for scanning.

    Don't do a table scan on InnoDB tables, if we can read the used
    parts of the row from any of the used index.
    This is because table scans uses index and we would not win
    anything by using a table scan.

    A word for word translation of the below if-statement in sergefp's
    understanding: we check if we should use table scan if:
    (1) The found 'ref' access produces more records than a table scan
        (or index scan, or quick select), or 'ref' is more expensive than
        any of them.
    (2) This doesn't hold: the best way to perform table scan is to to perform
        'range' access using index IDX, and the best way to perform 'ref' 
        access is to use the same index IDX, with the same or more key parts.
        (note: it is not clear how this rule is/should be extended to 
        index_merge quick selects). Also if we have a hash join we prefer that
        over a table scan
    (3) See above note about InnoDB.
    (4) NOT ("FORCE INDEX(...)" is used for table and there is 'ref' access
             path, but there is no quick select)
        If the condition in the above brackets holds, then the only possible
        "table scan" access method is ALL/index (there is no quick select).
        Since we have a 'ref' access path, and FORCE INDEX instructs us to
        choose it over ALL/index, there is no need to consider a full table
        scan.
    (5) Non-flattenable semi-joins: don't consider doing a scan of temporary
        table if we had an option to make lookups into it. In real-world cases,
        lookups are cheaper than full scans, but when the table is small, they
        can be [considered to be] more expensive, which causes lookups not to 
        be used for cases with small datasets, which is annoying.
  */
  if ((records >= s->found_records || best > s->read_time) &&            // (1)
      !(best_key && best_key->key == MAX_KEY) &&                         // (2)
      !(s->quick && best_key && s->quick->index == best_key->key &&      // (2)
        best_max_key_part >= s->table->quick_key_parts[best_key->key]) &&// (2)
      !((s->table->file->ha_table_flags() & HA_TABLE_SCAN_ON_INDEX) &&   // (3)
        ! s->table->covering_keys.is_clear_all() && best_key && !s->quick) &&// (3)
      !(s->table->force_index && best_key && !s->quick) &&               // (4)
      !(best_key && s->table->pos_in_table_list->jtbm_subselect))        // (5)
  {                                             // Check full join
    double rnd_records= matching_candidates_in_table(s, found_constraint,
                                                      use_cond_selectivity);

    /*
      Range optimizer never proposes a RANGE if it isn't better
      than FULL: so if RANGE is present, it's always preferred to FULL.
      Here we estimate its cost.
    */

    if (s->quick)
    {
      /*
        For each record we:
        - read record range through 'quick'
        - skip rows which does not satisfy WHERE constraints
        TODO: 
        We take into account possible use of join cache for ALL/index
        access (see first else-branch below), but we don't take it into 
        account here for range/index_merge access. Find out why this is so.
      */
      double cmp_time= (s->found_records - rnd_records)/(double) TIME_FOR_COMPARE;
      tmp= COST_MULT(record_count,
                     COST_ADD(s->quick->read_time, cmp_time));

      loose_scan_opt.check_range_access(join, idx, s->quick);
    }
    else
    {
      /* Estimate cost of reading table. */
      if (s->table->force_index && !best_key) // index scan
        tmp= s->table->file->read_time(s->ref.key, 1, s->records);
      else // table scan
        tmp= s->scan_time();

      if ((s->table->map & join->outer_join) || disable_jbuf)     // Can't use join cache
      {
        /*
          For each record we have to:
          - read the whole table record 
          - skip rows which does not satisfy join condition
        */
        double cmp_time= (s->records - rnd_records)/(double) TIME_FOR_COMPARE;
        tmp= COST_MULT(record_count, COST_ADD(tmp,cmp_time));
      }
      else
      {
        double refills= (1.0 + floor((double) cache_record_length(join,idx) *
                        (record_count /
                         (double) thd->variables.join_buff_size)));
        tmp= COST_MULT(tmp, refills);
        /* 
            We don't make full cartesian product between rows in the scanned
           table and existing records because we skip all rows from the
           scanned table, which does not satisfy join condition when 
           we read the table (see flush_cached_records for details). Here we
           take into account cost to read and skip these records.
        */
        double cmp_time= (s->records - rnd_records)/(double) TIME_FOR_COMPARE;
        tmp= COST_ADD(tmp, cmp_time);
      }
    }

    /* Splitting technique cannot be used with join cache */
    if (s->table->is_splittable())
      tmp+= s->table->get_materialization_cost();
    else
      tmp+= s->startup_cost;
    /*
      We estimate the cost of evaluating WHERE clause for found records
      as record_count * rnd_records / TIME_FOR_COMPARE. This cost plus
      tmp give us total cost of using TABLE SCAN
    */
    if (best == DBL_MAX ||
        COST_ADD(tmp, record_count/(double) TIME_FOR_COMPARE*rnd_records) <
         (best_key->is_for_hash_join() ? best_time :
          COST_ADD(best, record_count/(double) TIME_FOR_COMPARE*records)))
    {
      /*
        If the table has a range (s->quick is set) make_join_select()
        will ensure that this will be used
      */
      best= tmp;
      records= rnd_records;
      best_key= 0;
      /* range/index_merge/ALL/index access method are "independent", so: */
      best_ref_depends_map= 0;
      best_uses_jbuf= MY_TEST(!disable_jbuf && !((s->table->map &
                                                  join->outer_join)));
      spl_plan= 0;
    }
  }

  /* Update the cost information for the current partial plan */
  pos->records_read= records;
  pos->read_time=    best;
  pos->key=          best_key;
  pos->table=        s;
  pos->ref_depend_map= best_ref_depends_map;
  pos->loosescan_picker.loosescan_key= MAX_KEY;
  pos->use_join_buffer= best_uses_jbuf;
  pos->spl_plan= spl_plan;

  loose_scan_opt.save_to_position(s, loose_scan_pos);

  if (!best_key &&
      idx == join->const_tables &&
      s->table == join->sort_by_table &&
      join->unit->select_limit_cnt >= records)
    join->sort_by_table= (TABLE*) 1;  // Must use temporary table

  DBUG_VOID_RETURN;
}