setup_async_signals ()
{
#if defined (__BEOS__)
  set_signal_handler (SIGHUP, SIG_IGN);	/* they want csh-like behavior */
#endif

#if defined (JOB_CONTROL)
  if (job_control == 0)
#endif
    {
      set_signal_handler (SIGINT, SIG_IGN);
      set_signal_ignored (SIGINT);
      set_signal_handler (SIGQUIT, SIG_IGN);
      set_signal_ignored (SIGQUIT);
    }
}