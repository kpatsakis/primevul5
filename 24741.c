bool Item_func_interval::fix_length_and_dec()
{
  uint rows= row->cols();
  
  use_decimal_comparison= ((row->element_index(0)->result_type() ==
                            DECIMAL_RESULT) ||
                           (row->element_index(0)->result_type() ==
                            INT_RESULT));
  if (rows > 8)
  {
    bool not_null_consts= TRUE;

    for (uint i= 1; not_null_consts && i < rows; i++)
    {
      Item *el= row->element_index(i);
      not_null_consts&= el->const_item() && !el->is_null();
    }

    if (not_null_consts)
    {
      intervals= (interval_range*) current_thd->alloc(sizeof(interval_range) *
                                                         (rows - 1));
      if (!intervals)
        return TRUE;

      if (use_decimal_comparison)
      {
        for (uint i= 1; i < rows; i++)
        {
          Item *el= row->element_index(i);
          interval_range *range= intervals + (i-1);
          if ((el->result_type() == DECIMAL_RESULT) ||
              (el->result_type() == INT_RESULT))
          {
            range->type= DECIMAL_RESULT;
            range->dec.init();
            my_decimal *dec= el->val_decimal(&range->dec);
            if (dec != &range->dec)
            {
              range->dec= *dec;
            }
          }
          else
          {
            range->type= REAL_RESULT;
            range->dbl= el->val_real();
          }
        }
      }
      else
      {
        for (uint i= 1; i < rows; i++)
        {
          intervals[i-1].dbl= row->element_index(i)->val_real();
        }
      }
    }
  }
  maybe_null= 0;
  max_length= 2;
  used_tables_and_const_cache_join(row);
  not_null_tables_cache= row->not_null_tables();
  join_with_sum_func(row);
  with_param= with_param || row->with_param;
  with_field= with_field || row->with_field;
  return FALSE;
}