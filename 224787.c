restore_signal_mask (set)
     sigset_t *set;
{
  return (sigprocmask (SIG_SETMASK, set, (sigset_t *)NULL));
}