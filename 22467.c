connection_attempt_unref (gpointer pointer)
{
  ConnectionAttempt *attempt = pointer;
  if (g_ref_count_dec (&attempt->ref))
    {
      g_clear_object (&attempt->address);
      g_clear_object (&attempt->socket);
      g_clear_object (&attempt->connection);
      g_clear_object (&attempt->cancellable);
      if (attempt->timeout_source)
        {
          g_source_destroy (attempt->timeout_source);
          g_source_unref (attempt->timeout_source);
        }
      g_free (attempt);
    }
}