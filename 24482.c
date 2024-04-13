bool ha_maria::is_changed() const
{
  return file->state->changed;
}