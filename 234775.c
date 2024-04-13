static int compare_fields_by_table_order(Item *field1,
                                         Item *field2,
                                         void *table_join_idx)
{
  int cmp= 0;
  bool outer_ref= 0;
  Item *field1_real= field1->real_item();
  Item *field2_real= field2->real_item();

  if (field1->const_item() || field1_real->const_item())
    return -1;
  if (field2->const_item() || field2_real->const_item())
    return 1;
  Item_field *f1= (Item_field *) field1_real;
  Item_field *f2= (Item_field *) field2_real;
  if (f1->used_tables() & OUTER_REF_TABLE_BIT)
  {
    outer_ref= 1;
    cmp= -1;
  }
  if (f2->used_tables() & OUTER_REF_TABLE_BIT)
  {  
    outer_ref= 1;
    cmp++;
  }
  if (outer_ref)
    return cmp;
  JOIN_TAB **idx= (JOIN_TAB **) table_join_idx;
  
  JOIN_TAB *tab1= idx[f1->field->table->tablenr];
  JOIN_TAB *tab2= idx[f2->field->table->tablenr];
  
  /* 
    if one of the table is inside a merged SJM nest and another one isn't,
    compare SJM bush roots of the tables.
  */
  if (tab1->bush_root_tab != tab2->bush_root_tab)
  {
    if (tab1->bush_root_tab)
      tab1= tab1->bush_root_tab;

    if (tab2->bush_root_tab)
      tab2= tab2->bush_root_tab;
  }
  
  cmp= (int)(tab1 - tab2);

  if (!cmp)
  {
    /* Fields f1, f2 belong to the same table */

    JOIN_TAB *tab= idx[f1->field->table->tablenr];
    uint keyno= MAX_KEY;
    if (tab->ref.key_parts)
      keyno= tab->ref.key;
    else if (tab->select && tab->select->quick)
       keyno = tab->select->quick->index;
    if (keyno != MAX_KEY)
    {
      if (f1->field->part_of_key.is_set(keyno))
        cmp= -1;
      if (f2->field->part_of_key.is_set(keyno))
        cmp++;
      /*
        Here:
        if both f1, f2 are components of the key tab->ref.key then cmp==0,
        if only f1 is a component of the key then cmp==-1 (f1 is better),
        if only f2 is a component of the key then cmp==1, (f2 is better),
        if none of f1,f1 is component of the key cmp==0.
      */  
      if (!cmp)
      {
        KEY *key_info= tab->table->key_info + keyno;
        for (uint i= 0; i < key_info->user_defined_key_parts; i++)
	{
          Field *fld= key_info->key_part[i].field;
          if (fld->eq(f1->field))
	  {
	    cmp= -1; // f1 is better
            break;
          }
          if (fld->eq(f2->field))
	  {
	    cmp= 1;  // f2 is better
            break;
          }
        }
      }              
    }              
    if (!cmp)   
      cmp= f1->field->field_index-f2->field->field_index;
  }
  return cmp < 0 ? -1 : (cmp ? 1 : 0);
}