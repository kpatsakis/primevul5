connection_attempt_ref (ConnectionAttempt *attempt)
{
  g_ref_count_inc (&attempt->ref);
  return attempt;
}