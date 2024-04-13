bool DML_prelocking_strategy::
handle_routine(THD *thd, Query_tables_list *prelocking_ctx,
               Sroutine_hash_entry *rt, sp_head *sp, bool *need_prelocking)
{
  /*
    We assume that for any "CALL proc(...)" statement sroutines_list will
    have 'proc' as first element (it may have several, consider e.g.
    "proc(sp_func(...)))". This property is currently guaranted by the
    parser.
  */

  if (rt != (Sroutine_hash_entry*)prelocking_ctx->sroutines_list.first ||
      rt->mdl_request.key.mdl_namespace() != MDL_key::PROCEDURE)
  {
    *need_prelocking= TRUE;
    sp_update_stmt_used_routines(thd, prelocking_ctx, &sp->m_sroutines,
                                 rt->belong_to_view);
    (void)sp->add_used_tables_to_table_list(thd,
                                            &prelocking_ctx->query_tables_last,
                                            rt->belong_to_view);
  }
  sp->propagate_attributes(prelocking_ctx);
  return FALSE;
}