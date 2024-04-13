Item *eliminate_item_equal(THD *thd, COND *cond, COND_EQUAL *upper_levels,
                           Item_equal *item_equal)
{
  List<Item> eq_list;
  Item_func_eq *eq_item= 0;
  if (((Item *) item_equal)->const_item() && !item_equal->val_int())
    return new (thd->mem_root) Item_int(thd, (longlong) 0, 1);
  Item *item_const= item_equal->get_const();
  Item_equal_fields_iterator it(*item_equal);
  Item *head;
  TABLE_LIST *current_sjm= NULL;
  Item *current_sjm_head= NULL;

  DBUG_ASSERT(!cond ||
              cond->type() == Item::INT_ITEM ||
              (cond->type() == Item::FUNC_ITEM &&
               ((Item_func *) cond)->functype() == Item_func::EQ_FUNC) ||  
              (cond->type() == Item::COND_ITEM  && 
               ((Item_func *) cond)->functype() == Item_func::COND_AND_FUNC));
       
  /* 
    Pick the "head" item: the constant one or the first in the join order
    (if the first in the join order happends to be inside an SJM nest, that's
    ok, because this is where the value will be unpacked after
    materialization).
  */
  if (item_const)
    head= item_const;
  else
  {
    TABLE_LIST *emb_nest;
    head= item_equal->get_first(NO_PARTICULAR_TAB, NULL);
    it++;
    if ((emb_nest= embedding_sjm(head)))
    {
      current_sjm= emb_nest;
      current_sjm_head= head;
    }
  }

  Item *field_item;
  /*
    For each other item, generate "item=head" equality (except the tables that 
    are within SJ-Materialization nests, for those "head" is defined
    differently)
  */
  while ((field_item= it++))
  {
    Item_equal *upper= field_item->find_item_equal(upper_levels);
    Item *item= field_item;
    TABLE_LIST *field_sjm= embedding_sjm(field_item);
    if (!field_sjm)
    { 
      current_sjm= NULL;
      current_sjm_head= NULL;
    }      

    /* 
      Check if "field_item=head" equality is already guaranteed to be true 
      on upper AND-levels.
    */
    if (upper)
    {
      TABLE_LIST *native_sjm= embedding_sjm(item_equal->context_field);
      Item *upper_const= upper->get_const();
      if (item_const && upper_const)
      {
        /* 
          Upper item also has "field_item=const".
          Don't produce equality if const is equal to item_const.
        */
        Item_func_eq *func= new (thd->mem_root) Item_func_eq(thd, item_const, upper_const);
        func->set_cmp_func();
        func->quick_fix_field();
        if (func->val_int())
          item= 0;
      }
      else
      {
        Item_equal_fields_iterator li(*item_equal);
        while ((item= li++) != field_item)
        {
          if (embedding_sjm(item) == field_sjm && 
              item->find_item_equal(upper_levels) == upper)
            break;
        }
      }
      if (embedding_sjm(field_item) != native_sjm)
        item= NULL; /* Don't produce equality */
    }
    
    bool produce_equality= MY_TEST(item == field_item);
    if (!item_const && field_sjm && field_sjm != current_sjm)
    {
      /* Entering an SJM nest */
      current_sjm_head= field_item;
      if (!field_sjm->sj_mat_info->is_sj_scan)
        produce_equality= FALSE;
    }

    if (produce_equality)
    {
      if (eq_item && eq_list.push_back(eq_item, thd->mem_root))
        return 0;
      
      /*
        If we're inside an SJM-nest (current_sjm!=NULL), and the multi-equality
        doesn't include a constant, we should produce equality with the first
        of the equal items in this SJM (except for the first element inside the
        SJM. For that, we produce the equality with the "head" item).

        In other cases, get the "head" item, which is either first of the
        equals on top level, or the constant.
      */
      Item *head_item= (!item_const && current_sjm && 
                        current_sjm_head != field_item) ? current_sjm_head: head; 
      Item *head_real_item=  head_item->real_item();
      if (head_real_item->type() == Item::FIELD_ITEM)
        head_item= head_real_item;
      
      eq_item= new (thd->mem_root) Item_func_eq(thd, field_item->real_item(), head_item);

      if (!eq_item || eq_item->set_cmp_func())
        return 0;
      eq_item->quick_fix_field();
    }
    current_sjm= field_sjm;
  }

  /*
    We have produced zero, one, or more pair-wise equalities eq_i. We want to
    return an expression in form:

      cond AND eq_1 AND eq_2 AND eq_3 AND ...
    
    'cond' is a parameter for this function, which may be NULL, an Item_int(1),
    or an Item_func_eq or an Item_cond_and.

    We want to return a well-formed condition: no nested Item_cond_and objects,
    or Item_cond_and with a single child:
    - if 'cond' is an Item_cond_and, we add eq_i as its tail
    - if 'cond' is Item_int(1), we return eq_i
    - otherwise, we create our own Item_cond_and and put 'cond' at the front of
      it.
    - if we have only one condition to return, we don't create an Item_cond_and
  */

  if (eq_item && eq_list.push_back(eq_item, thd->mem_root))
    return 0;
  COND *res= 0;
  switch (eq_list.elements)
  {
  case 0:
    res= cond ? cond : new (thd->mem_root) Item_int(thd, (longlong) 1, 1);
    break;
  case 1:
    if (!cond || cond->type() ==  Item::INT_ITEM)
      res= eq_item;
    break;
  default:
    break;
  }
  if (!res) 
  {
    if (cond)
    {
      if (cond->type() == Item::COND_ITEM)
      {
        res= cond;
        ((Item_cond *) res)->add_at_end(&eq_list);
      }
      else if (eq_list.push_front(cond, thd->mem_root))
        return 0;
    }
  }  
  if (!res)
    res= new (thd->mem_root) Item_cond_and(thd, eq_list);
  if (res)
  {
    res->quick_fix_field();
    res->update_used_tables();
  }

  return res;
}