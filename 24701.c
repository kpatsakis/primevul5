  virtual bool val_native(THD *thd, Native *to)
  {
   /*
     The default implementation for the Items that do not need native format:
     - Item_basic_value (default implementation)
     - Item_copy
     - Item_exists_subselect
     - Item_sum_field
     - Item_sum_or_func (default implementation)
     - Item_proc
     - Item_type_holder (as val_xxx() are never called for it);

     These hybrid Item types override val_native():
     - Item_field
     - Item_param
     - Item_sp_variable
     - Item_ref
     - Item_cache_wrapper
     - Item_direct_ref
     - Item_direct_view_ref
     - Item_ref_null_helper
     - Item_name_const
     - Item_time_literal
     - Item_sum_or_func
         Note, these hybrid type Item_sum_or_func descendants
         override the default implementation:
         * Item_sum_hybrid
         * Item_func_hybrid_field_type
         * Item_func_min_max
         * Item_func_sp
         * Item_func_last_value
         * Item_func_rollup_const
   */
    DBUG_ASSERT(0);
    return null_value= true;
  }