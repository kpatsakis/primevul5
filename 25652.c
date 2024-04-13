Field *create_tmp_field(THD *thd, TABLE *table,Item *item, Item::Type type,
                        Item ***copy_func, Field **from_field,
                        Field **default_field,
                        bool group, bool modify_item,
                        bool table_cant_handle_bit_fields,
                        bool make_copy_field)
{
  Field *result;
  Item::Type orig_type= type;
  Item *orig_item= 0;

  if (type != Item::FIELD_ITEM &&
      item->real_item()->type() == Item::FIELD_ITEM)
  {
    orig_item= item;
    item= item->real_item();
    type= Item::FIELD_ITEM;
  }

  switch (type) {
  case Item::SUM_FUNC_ITEM:
  {
    result= item->create_tmp_field(group, table);
    if (!result)
      my_error(ER_OUT_OF_RESOURCES, MYF(ME_FATALERROR));
    return result;
  }
  case Item::FIELD_ITEM:
  case Item::DEFAULT_VALUE_ITEM:
  case Item::INSERT_VALUE_ITEM:
  case Item::TRIGGER_FIELD_ITEM:
  {
    Item_field *field= (Item_field*) item;
    bool orig_modify= modify_item;
    if (orig_type == Item::REF_ITEM)
      modify_item= 0;
    /*
      If item have to be able to store NULLs but underlaid field can't do it,
      create_tmp_field_from_field() can't be used for tmp field creation.
    */
    if (((field->maybe_null && field->in_rollup) ||      
	(thd->create_tmp_table_for_derived  &&    /* for mat. view/dt */
	 orig_item && orig_item->maybe_null)) &&         
        !field->field->maybe_null())
    {
      bool save_maybe_null= FALSE;
      /*
        The item the ref points to may have maybe_null flag set while
        the ref doesn't have it. This may happen for outer fields
        when the outer query decided at some point after name resolution phase
        that this field might be null. Take this into account here.
      */
      if (orig_item)
      {
        save_maybe_null= item->maybe_null;
        item->maybe_null= orig_item->maybe_null;
      }
      result= create_tmp_field_from_item(thd, item, table, NULL,
                                         modify_item);
      *from_field= field->field;
      if (result && modify_item)
        field->result_field= result;
      if (orig_item)
        item->maybe_null= save_maybe_null;
    } 
    else if (table_cant_handle_bit_fields && field->field->type() ==
             MYSQL_TYPE_BIT)
    {
      *from_field= field->field;
      result= create_tmp_field_from_item(thd, item, table, copy_func,
                                         modify_item);
      if (result && modify_item)
        field->result_field= result;
    }
    else
      result= create_tmp_field_from_field(thd, (*from_field= field->field),
                                          orig_item ? orig_item->name :
                                          item->name,
                                          table,
                                          modify_item ? field :
                                          NULL);
    if (orig_type == Item::REF_ITEM && orig_modify)
      ((Item_ref*)orig_item)->set_result_field(result);
    /*
      Fields that are used as arguments to the DEFAULT() function already have
      their data pointers set to the default value during name resolution. See
      Item_default_value::fix_fields.
    */
    if (orig_type != Item::DEFAULT_VALUE_ITEM && field->field->eq_def(result))
      *default_field= field->field;
    return result;
  }
  /* Fall through */
  case Item::FUNC_ITEM:
    if (((Item_func *) item)->functype() == Item_func::FUNC_SP)
    {
      Item_func_sp *item_func_sp= (Item_func_sp *) item;
      Field *sp_result_field= item_func_sp->get_sp_result_field();

      if (make_copy_field)
      {
        DBUG_ASSERT(item_func_sp->result_field);
        *from_field= item_func_sp->result_field;
      }
      else
      {
        *((*copy_func)++)= item;
      }

      Field *result_field=
        create_tmp_field_from_field(thd,
                                    sp_result_field,
                                    item_func_sp->name,
                                    table,
                                    NULL);

      if (modify_item)
        item->set_result_field(result_field);

      return result_field;
    }

    /* Fall through */
  case Item::COND_ITEM:
  case Item::SUBSELECT_ITEM:
  case Item::REF_ITEM:
  case Item::EXPR_CACHE_ITEM:
    if (make_copy_field)
    {
      DBUG_ASSERT(((Item_result_field*)item)->result_field);
      *from_field= ((Item_result_field*)item)->result_field;
    }
    /* Fall through */
  case Item::FIELD_AVG_ITEM:
  case Item::FIELD_STD_ITEM:
  case Item::PROC_ITEM:
  case Item::INT_ITEM:
  case Item::REAL_ITEM:
  case Item::DECIMAL_ITEM:
  case Item::STRING_ITEM:
  case Item::DATE_ITEM:
  case Item::NULL_ITEM:
  case Item::VARBIN_ITEM:
  case Item::CACHE_ITEM:
  case Item::WINDOW_FUNC_ITEM: // psergey-winfunc:
  case Item::PARAM_ITEM:
    return create_tmp_field_from_item(thd, item, table,
                                      (make_copy_field ? 0 : copy_func),
                                       modify_item);
  case Item::TYPE_HOLDER:  
    result= ((Item_type_holder *)item)->make_field_by_type(table);
    result->set_derivation(item->collation.derivation,
                           item->collation.repertoire);
    return result;
  default:					// Dosen't have to be stored
    return 0;
  }
}