int multi_update::prepare(List<Item> &not_used_values,
			  SELECT_LEX_UNIT *lex_unit)

{
  TABLE_LIST *table_ref;
  SQL_I_List<TABLE_LIST> update;
  table_map tables_to_update;
  Item_field *item;
  List_iterator_fast<Item> field_it(*fields);
  List_iterator_fast<Item> value_it(*values);
  uint i, max_fields;
  uint leaf_table_count= 0;
  List_iterator<TABLE_LIST> ti(updated_leaves);
  DBUG_ENTER("multi_update::prepare");

  if (prepared)
    DBUG_RETURN(0);
  prepared= true;

  thd->count_cuted_fields= CHECK_FIELD_WARN;
  thd->cuted_fields=0L;
  THD_STAGE_INFO(thd, stage_updating_main_table);

  tables_to_update= get_table_map(fields);

  if (!tables_to_update)
  {
    my_message(ER_NO_TABLES_USED, ER_THD(thd, ER_NO_TABLES_USED), MYF(0));
    DBUG_RETURN(1);
  }

  /*
    We gather the set of columns read during evaluation of SET expression in
    TABLE::tmp_set by pointing TABLE::read_set to it and then restore it after
    setup_fields().
  */
  while ((table_ref= ti++))
  {
    if (table_ref->is_jtbm())
      continue;

    TABLE *table= table_ref->table;
    if (tables_to_update & table->map)
    {
      DBUG_ASSERT(table->read_set == &table->def_read_set);
      table->read_set= &table->tmp_set;
      bitmap_clear_all(table->read_set);
    }
  }

  /*
    We have to check values after setup_tables to get covering_keys right in
    reference tables
  */

  int error= setup_fields(thd, Ref_ptr_array(),
                          *values, MARK_COLUMNS_READ, 0, NULL, 0);

  ti.rewind();
  while ((table_ref= ti++))
  {
    if (table_ref->is_jtbm())
      continue;

    TABLE *table= table_ref->table;
    if (tables_to_update & table->map)
    {
      table->read_set= &table->def_read_set;
      bitmap_union(table->read_set, &table->tmp_set);
    }
  }
  if (unlikely(error))
    DBUG_RETURN(1);    

  /*
    Save tables beeing updated in update_tables
    update_table->shared is position for table
    Don't use key read on tables that are updated
  */

  update.empty();
  ti.rewind();
  while ((table_ref= ti++))
  {
    /* TODO: add support of view of join support */
    if (table_ref->is_jtbm())
      continue;
    TABLE *table=table_ref->table;
    leaf_table_count++;
    if (tables_to_update & table->map)
    {
      TABLE_LIST *tl= (TABLE_LIST*) thd->memdup(table_ref,
						sizeof(*tl));
      if (!tl)
	DBUG_RETURN(1);
      update.link_in_list(tl, &tl->next_local);
      tl->shared= table_count++;
      table->no_keyread=1;
      table->covering_keys.clear_all();
      table->pos_in_table_list= tl;
      table->prepare_triggers_for_update_stmt_or_event();
      table->reset_default_fields();
    }
  }

  table_count=  update.elements;
  update_tables= update.first;

  tmp_tables = (TABLE**) thd->calloc(sizeof(TABLE *) * table_count);
  tmp_table_param = (TMP_TABLE_PARAM*) thd->calloc(sizeof(TMP_TABLE_PARAM) *
						   table_count);
  fields_for_table= (List_item **) thd->alloc(sizeof(List_item *) *
					      table_count);
  values_for_table= (List_item **) thd->alloc(sizeof(List_item *) *
					      table_count);
  if (unlikely(thd->is_fatal_error))
    DBUG_RETURN(1);
  for (i=0 ; i < table_count ; i++)
  {
    fields_for_table[i]= new List_item;
    values_for_table[i]= new List_item;
  }
  if (unlikely(thd->is_fatal_error))
    DBUG_RETURN(1);

  /* Split fields into fields_for_table[] and values_by_table[] */

  while ((item= (Item_field *) field_it++))
  {
    Item *value= value_it++;
    uint offset= item->field->table->pos_in_table_list->shared;
    fields_for_table[offset]->push_back(item, thd->mem_root);
    values_for_table[offset]->push_back(value, thd->mem_root);
  }
  if (unlikely(thd->is_fatal_error))
    DBUG_RETURN(1);

  /* Allocate copy fields */
  max_fields=0;
  for (i=0 ; i < table_count ; i++)
  {
    set_if_bigger(max_fields, fields_for_table[i]->elements + leaf_table_count);
    if (fields_for_table[i]->elements)
    {
      TABLE *table= ((Item_field*)(fields_for_table[i]->head()))->field->table;
      switch_to_nullable_trigger_fields(*fields_for_table[i], table);
      switch_to_nullable_trigger_fields(*values_for_table[i], table);
    }
  }
  copy_field= new (thd->mem_root) Copy_field[max_fields];
  DBUG_RETURN(thd->is_fatal_error != 0);
}