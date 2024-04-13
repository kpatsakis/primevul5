static void ignore_sigpipe()
{
  signal(SIGPIPE, SIG_IGN);
}