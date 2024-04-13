bool ha_maria::check_and_repair(THD *thd)
{
  int error, crashed;
  HA_CHECK_OPT check_opt;
  const CSET_STRING query_backup= thd->query_string;
  DBUG_ENTER("ha_maria::check_and_repair");

  check_opt.init();
  check_opt.flags= T_MEDIUM | T_AUTO_REPAIR;

  error= 1;
  if (!aria_readonly &&
      (file->s->state.changed & (STATE_CRASHED_FLAGS | STATE_MOVED)) ==
      STATE_MOVED)
  {
    /* Remove error about crashed table */
    thd->get_stmt_da()->clear_warning_info(thd->query_id);
    push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                        ER_CRASHED_ON_USAGE,
                        "Zerofilling moved table %s", table->s->path.str);
    sql_print_information("Zerofilling moved table:  '%s'",
                          table->s->path.str);
    if (!(error= zerofill(thd, &check_opt)))
      DBUG_RETURN(0);
  }

  /*
    if we got this far - the table is crashed.
    but don't auto-repair if maria_recover_options is not set
  */
  if (!maria_recover_options)
    DBUG_RETURN(error);

  error= 0;
  // Don't use quick if deleted rows
  if (!file->state->del && (maria_recover_options & HA_RECOVER_QUICK))
    check_opt.flags |= T_QUICK;

  thd->set_query((char*) table->s->table_name.str,
                 (uint) table->s->table_name.length, system_charset_info);

  if (!(crashed= maria_is_crashed(file)))
  {
    sql_print_warning("Checking table:   '%s'", table->s->path.str);
    crashed= check(thd, &check_opt);
  }

  if (crashed)
  {
    bool save_log_all_errors;
    sql_print_warning("Recovering table: '%s'", table->s->path.str);
    save_log_all_errors= thd->log_all_errors;
    thd->log_all_errors|= (thd->variables.log_warnings > 2);
    check_opt.flags=
      ((maria_recover_options & HA_RECOVER_BACKUP ? T_BACKUP_DATA : 0) |
       (maria_recover_options & HA_RECOVER_FORCE ? 0 : T_SAFE_REPAIR) |
       T_AUTO_REPAIR);
    if (repair(thd, &check_opt))
      error= 1;
    thd->log_all_errors= save_log_all_errors;
  }
  thd->set_query(query_backup);
  DBUG_RETURN(error);
}