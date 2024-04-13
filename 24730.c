  Item_splocal_with_delayed_data_type(THD *thd,
                                      const Sp_rcontext_handler *rh,
                                      const LEX_CSTRING *sp_var_name,
                                      uint sp_var_idx,
                                      uint pos_in_q, uint len_in_q)
   :Item_splocal(thd, rh, sp_var_name, sp_var_idx, &type_handler_null,
                 pos_in_q, len_in_q)
  { }