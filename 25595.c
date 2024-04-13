static bool check_simple_equality(THD *thd, const Item::Context &ctx,
                                  Item *left_item, Item *right_item,
                                  COND_EQUAL *cond_equal)
{
  Item *orig_left_item= left_item;
  Item *orig_right_item= right_item;
  if (left_item->type() == Item::REF_ITEM &&
      ((Item_ref*)left_item)->ref_type() == Item_ref::VIEW_REF)
  {
    if (((Item_ref*)left_item)->get_depended_from())
      return FALSE;
    if (((Item_direct_view_ref*)left_item)->get_null_ref_table() !=
        NO_NULL_TABLE && !left_item->real_item()->used_tables())
      return FALSE;
    left_item= left_item->real_item();
  }
  if (right_item->type() == Item::REF_ITEM &&
      ((Item_ref*)right_item)->ref_type() == Item_ref::VIEW_REF)
  {
    if (((Item_ref*)right_item)->get_depended_from())
      return FALSE;
    if (((Item_direct_view_ref*)right_item)->get_null_ref_table() !=
        NO_NULL_TABLE && !right_item->real_item()->used_tables())
      return FALSE;
    right_item= right_item->real_item();
  }
  if (left_item->type() == Item::FIELD_ITEM &&
      right_item->type() == Item::FIELD_ITEM &&
      !((Item_field*)left_item)->get_depended_from() &&
      !((Item_field*)right_item)->get_depended_from())
  {
    /* The predicate the form field1=field2 is processed */

    Field *left_field= ((Item_field*) left_item)->field;
    Field *right_field= ((Item_field*) right_item)->field;

    if (!left_field->eq_def(right_field))
      return FALSE;

    /* Search for multiple equalities containing field1 and/or field2 */
    bool left_copyfl, right_copyfl;
    Item_equal *left_item_equal=
               find_item_equal(cond_equal, left_field, &left_copyfl);
    Item_equal *right_item_equal= 
               find_item_equal(cond_equal, right_field, &right_copyfl);

    /* As (NULL=NULL) != TRUE we can't just remove the predicate f=f */
    if (left_field->eq(right_field)) /* f = f */
      return (!(left_field->maybe_null() && !left_item_equal)); 

    if (left_item_equal && left_item_equal == right_item_equal)
    {
      /* 
        The equality predicate is inference of one of the existing
        multiple equalities, i.e the condition is already covered
        by upper level equalities
      */
       return TRUE;
    }
      
    /* Copy the found multiple equalities at the current level if needed */
    if (left_copyfl)
    {
      /* left_item_equal of an upper level contains left_item */
      left_item_equal= new (thd->mem_root) Item_equal(thd, left_item_equal);
      left_item_equal->set_context_field(((Item_field*) left_item));
      cond_equal->current_level.push_back(left_item_equal, thd->mem_root);
    }
    if (right_copyfl)
    {
      /* right_item_equal of an upper level contains right_item */
      right_item_equal= new (thd->mem_root) Item_equal(thd, right_item_equal);
      right_item_equal->set_context_field(((Item_field*) right_item));
      cond_equal->current_level.push_back(right_item_equal, thd->mem_root);
    }

    if (left_item_equal)
    { 
      /* left item was found in the current or one of the upper levels */
      if (! right_item_equal)
        left_item_equal->add(orig_right_item, thd->mem_root);
      else
      {
        /* Merge two multiple equalities forming a new one */
        left_item_equal->merge(thd, right_item_equal);
        /* Remove the merged multiple equality from the list */
        List_iterator<Item_equal> li(cond_equal->current_level);
        while ((li++) != right_item_equal) ;
        li.remove();
      }
    }
    else
    { 
      /* left item was not found neither the current nor in upper levels  */
      if (right_item_equal)
        right_item_equal->add(orig_left_item, thd->mem_root);
      else 
      {
        /* None of the fields was found in multiple equalities */
        Item_equal *item_equal= new (thd->mem_root) Item_equal(thd,
                                                               orig_left_item,
                                                               orig_right_item,
                                                               FALSE);
        item_equal->set_context_field((Item_field*)left_item);
        cond_equal->current_level.push_back(item_equal, thd->mem_root);
      }
    }
    return TRUE;
  }

  {
    /* The predicate of the form field=const/const=field is processed */
    Item *const_item= 0;
    Item_field *field_item= 0;
    Item *orig_field_item= 0;
    if (left_item->type() == Item::FIELD_ITEM &&
        !((Item_field*)left_item)->get_depended_from() &&
        right_item->const_item() && !right_item->is_expensive())
    {
      orig_field_item= orig_left_item;
      field_item= (Item_field *) left_item;
      const_item= right_item;
    }
    else if (right_item->type() == Item::FIELD_ITEM &&
             !((Item_field*)right_item)->get_depended_from() &&
             left_item->const_item() && !left_item->is_expensive())
    {
      orig_field_item= orig_right_item;
      field_item= (Item_field *) right_item;
      const_item= left_item;
    }

    if (const_item &&
        field_item->field->test_if_equality_guarantees_uniqueness(const_item))
    {
      /*
        field_item and const_item are arguments of a scalar or a row
        comparison function:
          WHERE column=constant
          WHERE (column, ...) = (constant, ...)

        The owner comparison function has previously called fix_fields(),
        so field_item and const_item should be directly comparable items,
        field_item->cmp_context and const_item->cmp_context should be set.
        In case of string comparison, charsets and collations of
        field_item and const_item should have already be aggregated
        for comparison, all necessary character set converters installed
        and fixed.

        In case of string comparison, const_item can be either:
        - a weaker constant that does not need to be converted to field_item:
            WHERE latin1_field = 'latin1_const'
            WHERE varbinary_field = 'latin1_const'
            WHERE latin1_bin_field = 'latin1_general_ci_const'
        - a stronger constant that does not need to be converted to field_item:
            WHERE latin1_field = binary 0xDF
            WHERE latin1_field = 'a' COLLATE latin1_bin
        - a result of conversion (e.g. from the session character set)
          to the character set of field_item:
            WHERE latin1_field = 'utf8_string_with_latin1_repertoire'
      */
      bool copyfl;

      Item_equal *item_equal = find_item_equal(cond_equal,
                                               field_item->field, &copyfl);
      if (copyfl)
      {
        item_equal= new (thd->mem_root) Item_equal(thd, item_equal);
        cond_equal->current_level.push_back(item_equal, thd->mem_root);
        item_equal->set_context_field(field_item);
      }
      Item *const_item2= field_item->field->get_equal_const_item(thd, ctx,
                                                                 const_item);
      if (!const_item2)
        return false;

      if (item_equal)
      {
        /* 
          The flag cond_false will be set to 1 after this, if item_equal
          already contains a constant and its value is  not equal to
          the value of const_item.
        */
        item_equal->add_const(thd, const_item2);
      }
      else
      {
        item_equal= new (thd->mem_root) Item_equal(thd, const_item2,
                                                   orig_field_item, TRUE);
        item_equal->set_context_field(field_item);
        cond_equal->current_level.push_back(item_equal, thd->mem_root);
      }
      return TRUE;
    }
  }
  return FALSE;
}