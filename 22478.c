on_timer (GCancellable *cancel)
{
  g_cancellable_cancel (cancel);
  return G_SOURCE_REMOVE;
}