static int mark_recovery_start(const char* log_dir)
{
  int res;
  DBUG_ENTER("mark_recovery_start");
  if (!(maria_recover_options & HA_RECOVER_ANY))
    ma_message_no_user(ME_WARNING, "Please consider using option"
                       " --aria-recover-options[=...] to automatically check and"
                       " repair tables when logs are removed by option"
                       " --aria-force-start-after-recovery-failures=#");
  if (recovery_failures >= force_start_after_recovery_failures)
  {
    /*
      Remove logs which cause the problem; keep control file which has
      critical info like uuid, max_trid (removing control file may make
      correct tables look corrupted!).
    */
    char msg[100];
    res= translog_walk_filenames(log_dir, &translog_callback_delete_all);
    my_snprintf(msg, sizeof(msg),
                "%s logs after %u consecutive failures of"
                " recovery from logs",
                (res ? "failed to remove some" : "removed all"),
                recovery_failures);
    ma_message_no_user((res ? 0 : ME_WARNING), msg);
  }
  else
    res= ma_control_file_write_and_force(last_checkpoint_lsn, last_logno,
                                         max_trid_in_control_file,
                                         recovery_failures + 1);
  DBUG_RETURN(res);
}