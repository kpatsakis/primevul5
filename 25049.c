void Item_func::convert_const_compared_to_int_field(THD *thd)
{
  DBUG_ASSERT(arg_count >= 2); // Item_func_nullif has arg_count == 3
  if (!thd->lex->is_ps_or_view_context_analysis())
  {
    int field;
    if (args[field= 0]->real_item()->type() == FIELD_ITEM ||
        args[field= 1]->real_item()->type() == FIELD_ITEM)
    {
      Item_field *field_item= (Item_field*) (args[field]->real_item());
      if (((field_item->field_type() == MYSQL_TYPE_LONGLONG &&
            field_item->type_handler() != &type_handler_vers_trx_id) ||
           field_item->field_type() ==  MYSQL_TYPE_YEAR))
        convert_const_to_int(thd, field_item, &args[!field]);
    }
  }
}