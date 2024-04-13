setup_group(THD *thd, Ref_ptr_array ref_pointer_array, TABLE_LIST *tables,
	    List<Item> &fields, List<Item> &all_fields, ORDER *order,
	    bool *hidden_group_fields, bool from_window_spec)
{
  enum_parsing_place context_analysis_place=
                     thd->lex->current_select->context_analysis_place;
  *hidden_group_fields=0;
  ORDER *ord;

  if (!order)
    return 0;				/* Everything is ok */

  uint org_fields=all_fields.elements;

  thd->where="group statement";
  for (ord= order; ord; ord= ord->next)
  {
    if (find_order_in_list(thd, ref_pointer_array, tables, ord, fields,
                           all_fields, true, true, from_window_spec))
      return 1;
    (*ord->item)->marker= UNDEF_POS;		/* Mark found */
    if ((*ord->item)->with_sum_func && context_analysis_place == IN_GROUP_BY)
    {
      my_error(ER_WRONG_GROUP_FIELD, MYF(0), (*ord->item)->full_name());
      return 1;
    }
    if ((*ord->item)->with_window_func)
    {
      if (context_analysis_place == IN_GROUP_BY)
        my_error(ER_WRONG_PLACEMENT_OF_WINDOW_FUNCTION, MYF(0));
      else
        my_error(ER_WINDOW_FUNCTION_IN_WINDOW_SPEC, MYF(0));
      return 1;
    }
    if (from_window_spec && (*ord->item)->with_sum_func &&
        (*ord->item)->type() != Item::SUM_FUNC_ITEM)
      (*ord->item)->split_sum_func(thd, ref_pointer_array,
                                   all_fields, SPLIT_SUM_SELECT);
  }
  if (thd->variables.sql_mode & MODE_ONLY_FULL_GROUP_BY &&
      context_analysis_place == IN_GROUP_BY)
  {
    /*
      Don't allow one to use fields that is not used in GROUP BY
      For each select a list of field references that aren't under an
      aggregate function is created. Each field in this list keeps the
      position of the select list expression which it belongs to.

      First we check an expression from the select list against the GROUP BY
      list. If it's found there then it's ok. It's also ok if this expression
      is a constant or an aggregate function. Otherwise we scan the list
      of non-aggregated fields and if we'll find at least one field reference
      that belongs to this expression and doesn't occur in the GROUP BY list
      we throw an error. If there are no fields in the created list for a
      select list expression this means that all fields in it are used under
      aggregate functions.
    */
    Item *item;
    Item_field *field;
    int cur_pos_in_select_list= 0;
    List_iterator<Item> li(fields);
    List_iterator<Item_field> naf_it(thd->lex->current_select->join->non_agg_fields);

    field= naf_it++;
    while (field && (item=li++))
    {
      if (item->type() != Item::SUM_FUNC_ITEM && item->marker >= 0 &&
          !item->const_item() &&
          !(item->real_item()->type() == Item::FIELD_ITEM &&
            item->used_tables() & OUTER_REF_TABLE_BIT))
      {
        while (field)
        {
          /* Skip fields from previous expressions. */
          if (field->marker < cur_pos_in_select_list)
            goto next_field;
          /* Found a field from the next expression. */
          if (field->marker > cur_pos_in_select_list)
            break;
          /*
            Check whether the field occur in the GROUP BY list.
            Throw the error later if the field isn't found.
          */
          for (ord= order; ord; ord= ord->next)
            if ((*ord->item)->eq((Item*)field, 0))
              goto next_field;
          /*
            TODO: change ER_WRONG_FIELD_WITH_GROUP to more detailed
            ER_NON_GROUPING_FIELD_USED
          */
          my_error(ER_WRONG_FIELD_WITH_GROUP, MYF(0), field->full_name());
          return 1;
next_field:
          field= naf_it++;
        }
      }
      cur_pos_in_select_list++;
    }
  }
  if (org_fields != all_fields.elements)
    *hidden_group_fields=1;			// group fields is not used
  return 0;
}