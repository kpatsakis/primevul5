int ha_maria::indexes_are_disabled(void)
{
  return maria_indexes_are_disabled(file);
}