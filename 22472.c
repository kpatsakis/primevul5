on_connection_cancelled (GCancellable *cancellable,
                         gpointer      data)
{
  GCancellable *attempt_cancellable = data;

  g_cancellable_cancel (attempt_cancellable);
}