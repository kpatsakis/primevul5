int imap_wait_keepalive(pid_t pid)
{
  struct sigaction oldalrm;
  struct sigaction act;
  sigset_t oldmask;
  int rc;

  const bool c_imap_passive = cs_subset_bool(NeoMutt->sub, "imap_passive");
  cs_subset_str_native_set(NeoMutt->sub, "imap_passive", true, NULL);
  OptKeepQuiet = true;

  sigprocmask(SIG_SETMASK, NULL, &oldmask);

  sigemptyset(&act.sa_mask);
  act.sa_handler = mutt_sig_empty_handler;
#ifdef SA_INTERRUPT
  act.sa_flags = SA_INTERRUPT;
#else
  act.sa_flags = 0;
#endif

  sigaction(SIGALRM, &act, &oldalrm);

  const short c_imap_keepalive =
      cs_subset_number(NeoMutt->sub, "imap_keepalive");
  alarm(c_imap_keepalive);
  while ((waitpid(pid, &rc, 0) < 0) && (errno == EINTR))
  {
    alarm(0); /* cancel a possibly pending alarm */
    imap_keepalive();
    alarm(c_imap_keepalive);
  }

  alarm(0); /* cancel a possibly pending alarm */

  sigaction(SIGALRM, &oldalrm, NULL);
  sigprocmask(SIG_SETMASK, &oldmask, NULL);

  OptKeepQuiet = false;
  cs_subset_str_native_set(NeoMutt->sub, "imap_passive", c_imap_passive, NULL);

  return rc;
}