connection_attempt_remove (ConnectionAttempt *attempt)
{
  attempt->data->connection_attempts = g_slist_remove (attempt->data->connection_attempts, attempt);
  connection_attempt_unref (attempt);
}