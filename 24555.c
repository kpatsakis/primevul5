static int maria_hton_panic(handlerton *hton, ha_panic_function flag)
{
  /* If no background checkpoints, we need to do one now */
  int ret=0;

  if (!checkpoint_interval && !aria_readonly)
    ret= ma_checkpoint_execute(CHECKPOINT_FULL, FALSE);

  ret|= maria_panic(flag);

  maria_hton= 0;
  return ret;
}