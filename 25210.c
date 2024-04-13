bool Item_func_between::fix_length_and_dec_numeric(THD *thd)
{
  /* See the comment about the similar block in Item_bool_func2 */
  if (args[0]->real_item()->type() == FIELD_ITEM &&
      !thd->lex->is_ps_or_view_context_analysis())
  {
    Item_field *field_item= (Item_field*) (args[0]->real_item());
    if (field_item->field_type() ==  MYSQL_TYPE_LONGLONG ||
        field_item->field_type() ==  MYSQL_TYPE_YEAR)
    {
      const bool cvt_arg1= convert_const_to_int(thd, field_item, &args[1]);
      const bool cvt_arg2= convert_const_to_int(thd, field_item, &args[2]);
      if (cvt_arg1 && cvt_arg2)
      {
        // Works for all types
        m_comparator.set_handler(&type_handler_longlong);
      }
    }
  }
  return FALSE;
}