  Item_splocal_row_field(THD *thd,
                         const Sp_rcontext_handler *rh,
                         const LEX_CSTRING *sp_var_name,
                         const LEX_CSTRING *sp_field_name,
                         uint sp_var_idx, uint sp_field_idx,
                         const Type_handler *handler,
                         uint pos_in_q= 0, uint len_in_q= 0)
   :Item_splocal(thd, rh, sp_var_name, sp_var_idx, handler, pos_in_q, len_in_q),
    m_field_name(*sp_field_name),
    m_field_idx(sp_field_idx)
  { }