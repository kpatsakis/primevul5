double table_cond_selectivity(JOIN *join, uint idx, JOIN_TAB *s,
                              table_map rem_tables)
{
  uint16 ref_keyuse_steps_buf[MAX_REF_PARTS];
  uint   ref_keyuse_size= MAX_REF_PARTS;
  uint16 *ref_keyuse_steps= ref_keyuse_steps_buf;
  Field *field;
  TABLE *table= s->table;
  MY_BITMAP *read_set= table->read_set;
  double sel= s->table->cond_selectivity;
  POSITION *pos= &join->positions[idx];
  uint keyparts= 0;
  uint found_part_ref_or_null= 0;

  if (pos->key != 0)
  {
    /* 
      A ref access or hash join is used for this table. ref access is created
      from

        tbl.keypart1=expr1 AND tbl.keypart2=expr2 AND ...
      
      and it will only return rows for which this condition is satisified.
      Suppose, certain expr{i} is a constant. Since ref access only returns
      rows that satisfy
        
         tbl.keypart{i}=const       (*)

      then selectivity of this equality should not be counted in return value 
      of this function. This function uses the value of 
       
         table->cond_selectivity=selectivity(COND(tbl)) (**)
      
      as a starting point. This value includes selectivity of equality (*). We
      should somehow discount it. 
      
      Looking at calculate_cond_selectivity_for_table(), one can see that that
      the value is not necessarily a direct multiplicand in 
      table->cond_selectivity

      There are three possible ways to discount
      1. There is a potential range access on t.keypart{i}=const. 
         (an important special case: the used ref access has a const prefix for
          which a range estimate is available)
      
      2. The field has a histogram. field[x]->cond_selectivity has the data.
      
      3. Use index stats on this index:
         rec_per_key[key_part+1]/rec_per_key[key_part]

      (TODO: more details about the "t.key=othertable.col" case)
    */
    KEYUSE *keyuse= pos->key;
    KEYUSE *prev_ref_keyuse= keyuse;
    uint key= keyuse->key;
    bool used_range_selectivity= false;
    
    /*
      Check if we have a prefix of key=const that matches a quick select.
    */
    if (!is_hash_join_key_no(key))
    {
      key_part_map quick_key_map= (key_part_map(1) << table->quick_key_parts[key]) - 1;
      if (table->quick_rows[key] && 
          !(quick_key_map & ~table->const_key_parts[key]))
      {
        /* 
          Ok, there is an equality for each of the key parts used by the
          quick select. This means, quick select's estimate can be reused to
          discount the selectivity of a prefix of a ref access.
        */
        for (; quick_key_map & 1 ; quick_key_map>>= 1)
        {
          while (keyuse->table == table && keyuse->key == key && 
                 keyuse->keypart == keyparts)
          {
            keyuse++;
          }
          keyparts++;
        }
        /*
          Here we discount selectivity of the constant range CR. To calculate
          this selectivity we use elements from the quick_rows[] array.
          If we have indexes i1,...,ik with the same prefix compatible
          with CR any of the estimate quick_rows[i1], ... quick_rows[ik] could
          be used for this calculation but here we don't know which one was
          actually used. So sel could be greater than 1 and we have to cap it.
          However if sel becomes greater than 2 then with high probability
          something went wrong.
	*/
        sel /= (double)table->quick_rows[key] / (double) table->stat_records();
        set_if_smaller(sel, 1.0);
        used_range_selectivity= true;
      }
    }
    
    /*
      Go through the "keypart{N}=..." equalities and find those that were
      already taken into account in table->cond_selectivity.
    */
    keyuse= pos->key;
    keyparts=0;
    while (keyuse->table == table && keyuse->key == key)
    {
      if (!(keyuse->used_tables & (rem_tables | table->map)))
      {
        if (are_tables_local(s, keyuse->val->used_tables()))
	{
          if (is_hash_join_key_no(key))
	  {
            if (keyparts == keyuse->keypart)
              keyparts++;
          }
          else
	  {
            if (keyparts == keyuse->keypart &&
                !((keyuse->val->used_tables()) & ~pos->ref_depend_map) &&
                !(found_part_ref_or_null & keyuse->optimize))
	    {
              /* Found a KEYUSE object that will be used by ref access */
              keyparts++;
              found_part_ref_or_null|= keyuse->optimize & ~KEY_OPTIMIZE_EQ;
            }
          }

          if (keyparts > keyuse->keypart)
	  {
            /* Ok this is the keyuse that will be used for ref access */
            if (!used_range_selectivity && keyuse->val->const_item())
            { 
              uint fldno;
              if (is_hash_join_key_no(key))
                fldno= keyuse->keypart;
              else
                fldno= table->key_info[key].key_part[keyparts-1].fieldnr - 1;

              if (table->field[fldno]->cond_selectivity > 0)
	      {            
                sel /= table->field[fldno]->cond_selectivity;
                set_if_smaller(sel, 1.0);
              }
              /* 
               TODO: we could do better here:
                 1. cond_selectivity might be =1 (the default) because quick 
                    select on some index prevented us from analyzing 
                    histogram for this column.
                 2. we could get an estimate through this?
                     rec_per_key[key_part-1] / rec_per_key[key_part]
              */
            }
            if (keyparts > 1)
	    {
              /*
                Prepare to set ref_keyuse_steps[keyparts-2]: resize the array
                if it is not large enough
              */
              if (keyparts - 2 >= ref_keyuse_size)
              {
                uint new_size= MY_MAX(ref_keyuse_size*2, keyparts);
                void *new_buf;
                if (!(new_buf= my_malloc(sizeof(*ref_keyuse_steps)*new_size,
                                         MYF(0))))
                {
                  sel= 1.0; // As if no selectivity was computed
                  goto exit;
                }
                memcpy(new_buf, ref_keyuse_steps,
                       sizeof(*ref_keyuse_steps)*ref_keyuse_size);
                if (ref_keyuse_steps != ref_keyuse_steps_buf)
                  my_free(ref_keyuse_steps);

                ref_keyuse_steps= (uint16*)new_buf;
                ref_keyuse_size= new_size;
              }

              ref_keyuse_steps[keyparts-2]= (uint16)(keyuse - prev_ref_keyuse);
              prev_ref_keyuse= keyuse;
            }
          }
	}
      }
      keyuse++;
    }
  }
  else
  {
    /*
      The table is accessed with full table scan, or quick select.
      Selectivity of COND(table) is already accounted for in 
      matching_candidates_in_table().
    */
    sel= 1;
  }

  /* 
    If the field f from the table is equal to a field from one the
    earlier joined tables then the selectivity of the range conditions
    over the field f must be discounted.

    We need to discount selectivity only if we're using ref-based 
    access method (and have sel!=1).
    If we use ALL/range/index_merge, then sel==1, and no need to discount.
  */
  if (pos->key != NULL)
  {
    for (Field **f_ptr=table->field ; (field= *f_ptr) ; f_ptr++)
    {
      if (!bitmap_is_set(read_set, field->field_index) ||
          !field->next_equal_field)
        continue; 
      for (Field *next_field= field->next_equal_field; 
           next_field != field; 
           next_field= next_field->next_equal_field)
      {
        if (!(next_field->table->map & rem_tables) && next_field->table != table)
        { 
          if (field->cond_selectivity > 0)
	  {
            sel/= field->cond_selectivity;
            set_if_smaller(sel, 1.0);
          }
          break;
        }
      }
    }
  }

  sel*= table_multi_eq_cond_selectivity(join, idx, s, rem_tables,
                                        keyparts, ref_keyuse_steps);
exit:
  if (ref_keyuse_steps != ref_keyuse_steps_buf)
    my_free(ref_keyuse_steps);
  return sel;
}