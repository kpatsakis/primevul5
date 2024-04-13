bool ha_maria::is_crashed() const
{
  return (file->s->state.changed & (STATE_CRASHED_FLAGS | STATE_MOVED) ||
          (my_disable_locking && file->s->state.open_count));
}