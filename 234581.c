double table_multi_eq_cond_selectivity(JOIN *join, uint idx, JOIN_TAB *s,
                                       table_map rem_tables, uint keyparts,
                                       uint16 *ref_keyuse_steps)
{
  double sel= 1.0;
  COND_EQUAL *cond_equal= join->cond_equal;

  if (!cond_equal || !cond_equal->current_level.elements)
    return sel;

   if (!s->keyuse)
    return sel;

  Item_equal *item_equal;
  List_iterator_fast<Item_equal> it(cond_equal->current_level);
  TABLE *table= s->table;
  table_map table_bit= table->map;
  POSITION *pos= &join->positions[idx];
  
  while ((item_equal= it++))
  { 
    /* 
      Check whether we need to take into account the selectivity of
      multiple equality item_equal. If this is the case multiply
      the current value of sel by this selectivity
    */
    table_map used_tables= item_equal->used_tables();
    if (!(used_tables & table_bit))
      continue;
    if (item_equal->get_const())
      continue;

    bool adjust_sel= FALSE;
    Item_equal_fields_iterator fi(*item_equal);
    while((fi++) && !adjust_sel)
    {
      Field *fld= fi.get_curr_field();
      if (fld->table->map != table_bit)
        continue;
      if (pos->key == 0)
        adjust_sel= TRUE;
      else
      {
        uint i;
        KEYUSE *keyuse= pos->key;
        uint key= keyuse->key;
        for (i= 0; i < keyparts; i++)
	{
          if (i > 0)
            keyuse+= ref_keyuse_steps[i-1];
          uint fldno;
          if (is_hash_join_key_no(key))
	    fldno= keyuse->keypart;
          else
            fldno= table->key_info[key].key_part[i].fieldnr - 1;        
          if (fld->field_index == fldno)
            break;
        }
        keyuse= pos->key;

        if (i == keyparts)
	{
          /* 
            Field fld is included in multiple equality item_equal
            and is not a part of the ref key.
            The selectivity of the multiple equality must be taken
            into account unless one of the ref arguments is
            equal to fld.  
	  */
          adjust_sel= TRUE;
          for (uint j= 0; j < keyparts && adjust_sel; j++)
	  {
            if (j > 0)
              keyuse+= ref_keyuse_steps[j-1];  
            Item *ref_item= keyuse->val;
	    if (ref_item->real_item()->type() == Item::FIELD_ITEM)
	    {
              Item_field *field_item= (Item_field *) (ref_item->real_item());
              if (item_equal->contains(field_item->field))
                adjust_sel= FALSE;              
	    }
          }
        }          
      }
    }
    if (adjust_sel)
    {
      /* 
        If ref == 0 and there are no fields in the multiple equality
        item_equal that belong to the tables joined prior to s
        then the selectivity of multiple equality will be set to 1.0.
      */
      double eq_fld_sel= 1.0;
      fi.rewind();
      while ((fi++))
      {
        double curr_eq_fld_sel;
        Field *fld= fi.get_curr_field();
        if (!(fld->table->map & ~(table_bit | rem_tables)))
          continue;
        curr_eq_fld_sel= get_column_avg_frequency(fld) /
                         fld->table->stat_records();
        if (curr_eq_fld_sel < 1.0)
          set_if_bigger(eq_fld_sel, curr_eq_fld_sel);
      }
      sel*= eq_fld_sel;
    }
  } 
  return sel;
}