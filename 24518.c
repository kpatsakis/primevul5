static int mark_recovery_success(void)
{
  /* success of recovery, reset recovery_failures: */
  int res;
  DBUG_ENTER("mark_recovery_success");
  res= ma_control_file_write_and_force(last_checkpoint_lsn, last_logno,
                                       max_trid_in_control_file, 0);
  DBUG_RETURN(res);
}