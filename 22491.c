connection_attempt_new (void)
{
  ConnectionAttempt *attempt = g_new0 (ConnectionAttempt, 1);
  g_ref_count_init (&attempt->ref);
  return attempt;
}